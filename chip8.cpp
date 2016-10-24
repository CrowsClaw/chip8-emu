#include "chip8.h"

#define X(A) (((A) & 0x0F00) >> 8)
#define Y(A) (((A) & 0x00F0) >> 4)

void chip8::setKey(int index, bool isKeyPressed)
{
	key[index] = isKeyPressed;
}

void chip8::init()
{
	PC = 0x200;
	opcode = 0;
	I = 0;
	stackPointer = 0;
	//erm... may not want to abuse memset here
	memset(memory, 0, sizeof(memory));
	memset(stack, 0, sizeof(stack));
	memset(V, 0, sizeof(V));
	memset(gfx, 0, sizeof(gfx));
	memset(key, 0, sizeof(key));
	for(int i = 0; i < 80; i++)
	{
		memory[i] = chipFontSet[i];
	}

	delayTimer = 0;
	soundTimer = 0; 
	canDraw = true; //initial draw flag
	srand(time(NULL)); //seed the random number generator
}

bool chip8::loadGame(const std::string filename)
{
	FILE* input = fopen(filename.c_str(), "rb");
	if(!input)
	{
		return false; //maybe add some logging here
	}
	
	fseek(input, 0, SEEK_END); //advance the file to the end so we can get the real size
	int gameSize = ftell(input); //now we know the game size
	fseek(input, 0, SEEK_SET); //now rewind so we can read from the file

	byte* fileBuffer = (byte*)malloc(gameSize);
	fread(fileBuffer, 1, gameSize, input);

	for(int i = 0; i < gameSize; i++)
	{
		memory[i + 512] = fileBuffer[i];
	}

	fclose(input); //close file buffer

	return true;

}

void chip8::doEmulation()
{
	opcode = memory[PC] << 8 | memory[PC + 1];

	//printf("%d", opcode);
	switch(opcode & 0xF000)
	{
		case 0x0000:
			{
			switch(opcode & 0x000F)
				{
				case 0x0000: //clears the screen
					{
						for(int i = 0; i < (64 * 32); i++)
						{
							gfx[i] = 0;
						}
						PC += 2;
					} break;

				case 0x000E: //returns from subroutine
					{
						--stackPointer;
						PC = stack[stackPointer];
						PC += 2;
					} break;
				}

			} break;

		case 0x1000: //jumps to address NNN
			{
				PC = opcode & 0x0FFF;
			} break;
		
		case 0x2000: //calls the subroutine at address NNN
			{
				stack[stackPointer] = PC;
				++stackPointer;
				PC = opcode & 0x0FFF;
			} break;
		
		case 0x3000: //Skips the next instruction if V[X] equals NN
			{
				if(V[X(opcode)] == (opcode & 0x00FF))
				{
					PC += 4;
				}
				else
				{
					PC += 2;
				}
						
			} break;

		case 0x4000: //Skips the next instruction if V[X] does not equal NN
			{
				if(V[X(opcode)] != (opcode & 0x00FF))
				{
					PC += 4;
				}
				else
				{
					PC += 2;
				}
			} break;

		case 0x5000: //Skips the next instruction if V[X] V[Y]
			{
				if(V[X(opcode)] == V[Y(opcode)])
				{
					PC += 4;
				}

				else
				{
					PC += 2;
				}
			} break;


		case 0x6000: //sets V[X] to NN
			{
				V[X(opcode)] = (opcode & 0x00FF);
				PC += 2;
			}break;

		case 0x7000: //Adds NN to V[X]
			{
				V[X(opcode)] += (opcode & 0x00FF);
				PC += 2;
			} break;

		case 0x8000:
			{
				switch(opcode & 0x000F)
				{
					case 0x0000: //sets V[X] to V[Y]
						{
							V[X(opcode)] = V[Y(opcode)];
							PC += 2;
						} break;

					case 0x0001: //Does the bitwise OR operation on V[X], ORing V[Y] to V[X] 
						{
							V[X(opcode)] |= V[Y(opcode)];
							PC += 2;
						} break;

					case 0x0002: //Does the bitwise AND operation on V[X], ANDing V[Y] to V[X]
						{
							V[X(opcode)] &= V[Y(opcode)];
							PC += 2;
						} break;
					case 0x0003: //Does the bitwise XOR operation on V[X], XORing V[Y] to V[X]
						{
							V[X(opcode)] ^= V[Y(opcode)];
							PC += 2;
						} break;

					case 0x0004: //adds V[Y] to V[X]. Sets V[0xF] to 1 if there is carry, zero if there isnt.
						{
							if(V[Y(opcode)] > (0xFF - V[X(opcode)]))
							{
								V[0xF] = 1;
							}

							else
							{
								V[0xF] = 0;
							}
							
						V[X(opcode)] += V[Y(opcode)];
						PC += 2;
						
						} break;

					case 0x0005: //Subtracts V[Y] from V[X]. Sets V[0xF] to 0 if there is borrow, 1 if there isnt. 
						{
							if(V[Y(opcode)] > V[X(opcode)])
							{
								V[0xF] = 0;
							}
							else
							{
								V[0xF] = 1;
							}
							V[X(opcode)] -= V[Y(opcode)];
							PC += 2;
						} break;
					case 0x0006: //Shifts V[X] right by one. V[0xF] is set to the value of the least significant bit before the shift occurs.
						{
							V[0xF] = (V[X(opcode)] & 0x1);
							V[X(opcode)] >>= 1;
							PC += 2;
						} break;
					case 0x0007: //Sets V[X] to V[Y] - V[X]. Sets V[0xF] to 0 if there is borrow, 1 if there isnt.
						{ 	if(V[X(opcode)] > V[Y(opcode)])
							{
								V[0xF] = 0;
							}

							else
							{
								V[0xF] = 1;
							}
							V[X(opcode)] = V[Y(opcode)] - V[X(opcode)];
							PC += 2;
						} break;

					case 0x000E: //shifts V[X] to the left by one. V[0xF] is set to the value of the most significant bit before the shift occurs.
						{
							
							V[0xF] = V[X(opcode)] >> 7;
							V[X(opcode)] <<= 1;
							PC += 2;
						} break;
						default:
						printf ("Unknown opcode [0x8000]: 0x%X\n", opcode);

				}
			} break;

		case 0x9000: //Skips the next insttruction if V[X] is not equal to V[Y]
			{
				if(V[X(opcode)] != V[Y(opcode)])
				{
					PC += 4;
				}
				else
				{
					PC += 2;
				}
			} break;

		case 0xA000: //ANNN Sets I register to addres NNN
			{
				I = opcode & 0x0FFF;
				PC += 2;
			} break;

		case 0xB000: //Jump to address NNN plus V[0]
			{
				PC = (opcode & 0x0FFF) + V[0];
			} break;

		case 0xC000: //Sets V[X] to the result of a bitwise operation on a random number and NN
			{
				V[X(opcode)] = ((opcode & 0x00FF) & (rand() % 0xFF));
				PC += 2;
			} break;

		case 0xD000: //Draw a sprite at coord (V[X], V[Y]).
			{
				word xcoord = V[X(opcode)];
				word ycoord = V[Y(opcode)];
				word height = opcode & 0x000F;
				word pixel;

				V[0xF] = 0;
				for(int yline = 0; yline < height; yline++)
				{
					pixel = memory[I + yline];
					for(int xline = 0; xline < 8; xline++)
					{
						if( (pixel & (0x80 >> xline)) != 0)
						{
							if(gfx[(xcoord + xline + ((ycoord + yline) * 64))] == 1)
							{
								V[0xF] = 1;
							}
						   gfx[xcoord  + xline + ((ycoord + yline) * 64)] ^= 1;
						}
					}
				}


				canDraw = true;
				PC += 2;

			} break;

		case 0xE000: 
			{
			    switch(opcode & 0x00FF)
			    {
				    case 0x009E: //Skips the next instruction if they key stored in V[X] is pressed
					    {
						    if(key[V[X(opcode)]])
						    {
							    PC += 4;
						    }

						    else
						    {
							    PC += 2;
						    }
					    } break;

				    case 0x00A1: //skips the next instruction if the key stored in V[X] is not pressed
					    {
						if(!key[V[X(opcode)]] )
						{
							PC += 4;
						}
						else
						{
							PC += 2;
						}
					    } break;

					    default:
					printf ("Unknown opcode [0xE000]: 0x%X\n", opcode);
			    }
			} break;

		case 0xF000:
			{
			    switch(opcode & 0x00FF)
			    {
				    case 0x0007: //Sets V[X] to the value of the delay timer
					    {
							V[X(opcode)] = delayTimer;
							PC += 2;
					    } break;
				    case 0x000A: //A key pressed is awaited, then stored in V[X]
					    {
					    	bool keyPressed = false;
					    	for(int i = 0; i < 16; ++i)
					    	{
					    		if(key[i])
					    		{
					    			V[X(opcode)] = i;
					    			keyPressed = true;
					    		}
					    	}
					    		if(!keyPressed)
					    		{
					    			return;
					    		}
					    		PC += 2;
					    } break;

				    case 0x0015: //Sets the delay timer to V[X]
					    {
							delayTimer = V[X(opcode)];
							PC += 2;
					    } break;

				    case 0x0018: //Sets the sound timer to V[X]
					    {
							soundTimer = V[X(opcode)];
							PC += 2;
					    } break;

				    case 0x001E: //Adds V[X] to I. Checks for carry, sets V[X] to 1 if there is carry or 0 if there isnt
					    {
						    if((I + V[X(opcode)]) > 0xFFF)
						    {
							    V[0xF] = 1;
						    }

						    else
						    {
							    V[0xF] = 0;
						    }

						I += V[X(opcode)];
						PC += 2;
					    } break;

				    case 0x0029: //Sets I to the location of the sprite for the character in V[X]
					    {
						I = V[X(opcode)] * 0x5;
						PC += 2;
					    } break;

				   case 0x0033: /*Stores the binary coded decimal representation of V[X], 
				   				with the most significant of the three digits at the address in I,
				   				the middle digit at I plus one, and the least significant digit at I plus 2. 
				   				*/
					    {
						memory[I] = V[X(opcode)] / 100;
						memory[I + 1] = (V[X(opcode)] / 10) % 10;
						memory[I + 2] = (V[X(opcode)] % 100) % 10;
						PC += 2;
					    } break;

				   case 0x0055: //Stores V[0] to V[0xF] in memory starting at address I
					    {
						for(int i = 0; i <= X(opcode); ++i)
						{
							memory[I + i] = V[i];
						}

						I += X(opcode) + 1;
						PC += 2;
					    } break;

				   case 0x0065: // fils V[0] to V[0xF] with values from memory starting at address I.
					    {
						for(int i = 0; i <= X(opcode); ++i)
						{
							V[i] = memory[I + i];
						}

						I += X(opcode) + 1;
						PC += 2;
					    } break;

					default:
					    printf("Unknown opcode, (0xF000): 0x%x\n", opcode);
			    }
			} break;

			default:
			printf ("Unknown opcode: 0x%X\n", opcode);

	}

	if(delayTimer > 0) //always rember to update your timers after an instruction has been executed
	{
		--delayTimer;
	}

	if(soundTimer > 0)
	{
		if (soundTimer == 1)
		{
			printf("SOUND\n"); //the poor mans audio
		}
		--soundTimer;
	}

}

#ifndef CHIP_8
#define CHIP_8
#include <stdio.h> //for C-style file reading, srand, rand
#include <stdlib.h> //for malloc 
#include <string> //for actual strings
#include <string.h> //for memset, 
#include <time.h> //for time struct


typedef unsigned short word; //lets make our lives so much easier
typedef unsigned char byte; //same

#define CHIP_MEM_SIZE 4096 //the size of memory for the chip 8 system, in a easy to read format
#define CHIP_REG_SIZE 16 //the size of the registers

class chip8
{
	public:
		void init();
		bool loadGame(const std::string filename);
		void doEmulation();
		bool canDraw;
		byte gfx[64 * 32];
		byte key[16];
		void setKey(int index, bool isKeyPressed);

	private:
	word opcode; //the opcode to work on
	byte memory[CHIP_MEM_SIZE]; //RAM!
	byte V[CHIP_REG_SIZE]; //the registers
	word I, PC; //the index register and the program counter
	byte delayTimer, soundTimer;
	word stack[16];
	word stackPointer;
	byte chipFontSet[80] =
	{ 
	  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	  0x20, 0x60, 0x20, 0x20, 0x70, // 1
	  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};


};

#endif

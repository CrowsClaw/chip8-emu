#include <SFML/Graphics.hpp>
#include "chip8.h"
/*
Credits to https://github.com/aaruel/CHIP8-SFML
for showing how to implement drawing and user input!
*/



void drawEmulator(chip8& chip, sf::RenderWindow& window, int xScale, int yScale);

void checkInput(chip8& chip);

int main(int argc, char** argv)
{
	
	chip8 chip;
	chip.init();
	std::string game(argv[1]);
	if(!chip.loadGame(game) && argc < 2 || argc > 3)
	{
		printf("Game name could not be properly parsed!");
		return EXIT_FAILURE;
	}

	sf::RenderWindow window (sf::VideoMode(640, 320), "Chip 8 emulator!");
	


	while(window.isOpen())
	{
		sf::Event evnt;

		while(window.pollEvent(evnt))
		{
			if (evnt.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		chip.doEmulation();
		

		window.clear();
		checkInput(chip);
		drawEmulator(chip, window, 10, 10);
		window.display();
		window.clear();

		sf::sleep(sf::milliseconds(1));
	}



	return 0;
}


void drawEmulator(chip8& chip, sf::RenderWindow& window, int xScale, int yScale)
{
	sf::RectangleShape rect;
	rect.setFillColor(sf::Color::White);
	rect.setSize(sf::Vector2f(1 * xScale, 1 * yScale));

	for(int i = 0; i < (64 * 32); i++)
	{
		int xcoord = i % 64;
		int ycoord = i / 64;

		rect.setPosition(xcoord * xScale, ycoord * yScale);

		if(chip.gfx[i])
		{
			window.draw(rect);
		}
	}
	
}

void checkInput(chip8& chip)
{

	chip.setKey(0x1, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1));
	chip.setKey(0x2, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2));
	chip.setKey(0x3, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3));
	chip.setKey(0xC, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4));

	chip.setKey(0x4, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q));
	chip.setKey(0x5, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W));
	chip.setKey(0x6, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E));
	chip.setKey(0xD, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R));

	chip.setKey(0x7, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A));
    chip.setKey(0x8, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S));
    chip.setKey(0x9, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D));
    chip.setKey(0xE, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F));
    
    chip.setKey(0xA, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z));
    chip.setKey(0x0, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X));
    chip.setKey(0xB, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C));
    chip.setKey(0xF, sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V));
}


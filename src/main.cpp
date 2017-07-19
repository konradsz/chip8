#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <SFML/Graphics.hpp>

#include "Chip8.hpp"

const float TILE_SIZE = 10.0f;

void draw(sf::RenderWindow& window, unsigned char* display)
{
    for (int i = 0; i < DISPLAY_WIDTH; ++i)
    {
        for (int j = 0; j < DISPLAY_HEIGHT; ++j)
        {
            if (display[i + j * DISPLAY_WIDTH] != 0)
            {
                sf::RectangleShape shape({ TILE_SIZE, TILE_SIZE });
                shape.setPosition({ i * TILE_SIZE, j * TILE_SIZE });
                shape.setFillColor({ 255, 255, 255 });
                window.draw(shape);
            }
        }
    }
}

int main()
{
    Chip8 chip;
    chip.initialize();
    //chip.loadProgram("mathMaze.ch8");
    //chip.loadProgram("invaders.c8");
    //chip.loadProgram("tetris.c8");
    chip.loadProgram("../roms/R-ROULETTE");

    sf::RenderWindow window(sf::VideoMode(DISPLAY_WIDTH * TILE_SIZE, DISPLAY_HEIGHT * TILE_SIZE), "Chip8");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::Clock clock;
    sf::Time lag;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Num1:
                    chip.setKeyState(0x1, event.type);
                    break;
                case sf::Keyboard::Num2:
                    chip.setKeyState(0x2, event.type);
                    break;
                case sf::Keyboard::Num3:
                    chip.setKeyState(0x3, event.type);
                    break;
                case sf::Keyboard::Num4:
                    chip.setKeyState(0xC, event.type);
                    break;
                case sf::Keyboard::Q:
                    chip.setKeyState(0x4, event.type);
                    break;
                case sf::Keyboard::W:
                    chip.setKeyState(0x5, event.type);
                    break;
                case sf::Keyboard::E:
                    chip.setKeyState(0x6, event.type);
                    break;
                case sf::Keyboard::R:
                    chip.setKeyState(0xD, event.type);
                    break;
                case sf::Keyboard::A:
                    chip.setKeyState(0x7, event.type);
                    break;
                case sf::Keyboard::S:
                    chip.setKeyState(0x8, event.type);
                    break;
                case sf::Keyboard::D:
                    chip.setKeyState(0x9, event.type);
                    break;
                case sf::Keyboard::F:
                    chip.setKeyState(0xE, event.type);
                    break;
                case sf::Keyboard::Z:
                    chip.setKeyState(0xA, event.type);
                    break;
                case sf::Keyboard::X:
                    chip.setKeyState(0x0, event.type);
                    break;
                case sf::Keyboard::C:
                    chip.setKeyState(0xB, event.type);
                    break;
                case sf::Keyboard::V:
                    chip.setKeyState(0xF, event.type);
                    break;
                }
            }
        }

        sf::Time elapsed = clock.getElapsedTime();
        sf::Time time = elapsed + lag;

        const int NUMBER_OF_OPCODES_PER_SECOND = 600;
        const float TIMER_FREQUENCY = 60.0f; // 60 Hz
        const int NUMBER_OF_OPCODES_PER_FRAME = NUMBER_OF_OPCODES_PER_SECOND / TIMER_FREQUENCY;
        const sf::Time tickInterval = sf::microseconds(static_cast<sf::Int64>(1000000.0f / TIMER_FREQUENCY));

        if (time > tickInterval)
        {
            chip.emulateCycle(NUMBER_OF_OPCODES_PER_FRAME);
            lag = time - tickInterval;
            clock.restart();
        }

        if (chip.draw())
        {
            window.clear();
            draw(window, chip.getDisplay());

            //window.draw(shape);
            window.display();
        }
    }

    return 0;
}

#define _SCL_SECURE_NO_WARNINGS
#include "Chip8.hpp"

#include <fstream>
#include <algorithm>
#include <iostream>
#include <map>

#include <SFML/Graphics.hpp>

const float TILE_SIZE = 10.0f;

Chip8::Chip8() :
    window{ sf::VideoMode(DISPLAY_WIDTH * static_cast<unsigned>(TILE_SIZE),
                          DISPLAY_HEIGHT * static_cast<unsigned>(TILE_SIZE)),
                          "Chip8" }
{
}

void Chip8::loadROM(const std::string& fileName)
{
    std::ifstream program(fileName.c_str(), std::ios::binary);
    if (program.is_open())
    {
        std::vector<char> buffer{
            std::istreambuf_iterator<char>(program),
            std::istreambuf_iterator<char>()
        };
        if (buffer.size() <= MEMORY_SIZE - PROGRAM_MEMORY_OFFSET)
        {
            std::copy(std::begin(buffer), std::end(buffer),
                std::begin(cpu.memory) + PROGRAM_MEMORY_OFFSET);

            std::cout << "ROM " << fileName << " loaded, size: " << buffer.size() << std::endl;
        }
        else
        {
            std::cerr << "ROM image is too big: " << buffer.size() << " (max "
                << MEMORY_SIZE - PROGRAM_MEMORY_OFFSET << ")" << std::endl;
        }
    }
    else
    {
        std::cerr << "Unable to open file: " << fileName << std::endl;
    }
}

void Chip8::run()
{
    sf::Clock clock;
    sf::Time lag;

    while (window.isOpen())
    {
        handleInput();

        sf::Time elapsed = clock.getElapsedTime();
        sf::Time time = elapsed + lag;
        
        const unsigned int CYCLES_PER_SECOND = 600; // 600 Hz
        const unsigned int TIMER_FREQUENCY = 60; // 60 Hz
        const unsigned int CYCLES_PER_FRAME = CYCLES_PER_SECOND / TIMER_FREQUENCY;
        const sf::Time tickInterval = sf::microseconds(static_cast<sf::Int64>(1000000.0f / TIMER_FREQUENCY));

        if (time > tickInterval)
        {
            for (unsigned int i = 0; i < CYCLES_PER_FRAME; ++i)
            {
                cpu.emulateCycle();
            }
            
            if (cpu.playSound())
            {
                std::cout << "BEEP" << std::endl; // playSound!
            }

            cpu.decrementTimers();

            lag = time - tickInterval;
            clock.restart();
        }

        draw();
    }
}

void Chip8::draw()
{
    if (cpu.redraw())
    {
        window.clear();

        for (unsigned int i = 0; i < DISPLAY_WIDTH; ++i)
        {
            for (unsigned int j = 0; j < DISPLAY_HEIGHT; ++j)
            {
                if (cpu.display[i + j * DISPLAY_WIDTH] != 0)
                {
                    sf::RectangleShape shape({ TILE_SIZE, TILE_SIZE });
                    shape.setPosition({ i * TILE_SIZE, j * TILE_SIZE });
                    shape.setFillColor({ 255, 255, 255 });
                    window.draw(shape);
                }
            }
        }

        window.display();
    }
}

void Chip8::handleInput()
{
    static const std::map<sf::Keyboard::Key, uint8_t> keyCodeMap =
    {
        { sf::Keyboard::X,    0x0 },
        { sf::Keyboard::Num1, 0x1 },
        { sf::Keyboard::Num2, 0x2 },
        { sf::Keyboard::Num3, 0x3 },
        { sf::Keyboard::Q,    0x4 },
        { sf::Keyboard::W,    0x5 },
        { sf::Keyboard::E,    0x6 },
        { sf::Keyboard::A,    0x7 },
        { sf::Keyboard::S,    0x8 },
        { sf::Keyboard::D,    0x9 },
        { sf::Keyboard::Z,    0xA },
        { sf::Keyboard::C,    0xB },
        { sf::Keyboard::Num4, 0xC },
        { sf::Keyboard::R,    0xD },
        { sf::Keyboard::F,    0xE },
        { sf::Keyboard::V,    0xF }
    };

    auto isPressed = [](sf::Event::EventType type) { return (type == sf::Event::KeyPressed) ? 1 : 0; };

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
            default:
                const auto it = keyCodeMap.find(event.key.code);
                if (it != keyCodeMap.end())
                {
                    cpu.keypad[it->second] = isPressed(event.type);
                }
                break;
            }
        }
    }
}

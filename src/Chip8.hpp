#pragma once

#include <string>
#include <SFML/Graphics.hpp>

#include "CPU.hpp"

class Chip8
{
public:
    Chip8();
    void loadROM(const std::string& fileName);
    void run();

private:
    void draw();
    void handleInput();

private:
    CPU cpu;
    sf::RenderWindow window;
};

#pragma once

#include <string>
#include <random>
#include <SFML/Window/Event.hpp>

const unsigned MEMORY_SIZE = 4096;
const unsigned short PROGRAM_MEMORY_OFFSET = 0x200;

const unsigned DISPLAY_WIDTH = 64;
const unsigned DISPLAY_HEIGHT = 32;

class Chip8
{
public:
    Chip8();
    void loadProgram(const std::string& fileName);
    void initialize();
    void emulateCycle(int numberOfOpcodes);
    bool draw();
    unsigned char* getDisplay();
    void setKeyState(unsigned short keyCode, sf::Event::EventType eventType);

private:
    void processOpcode(unsigned short opcode);

private:
    // use uint16 etc
    unsigned char memory[MEMORY_SIZE];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned short stack[16];
    unsigned char sp;
    unsigned char display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    unsigned char keypad[16];
    unsigned char delayTimer;
    unsigned char soundTimer;

    bool drawFlag;

    std::mt19937 engine;
    std::uniform_int_distribution<> distribution;
};

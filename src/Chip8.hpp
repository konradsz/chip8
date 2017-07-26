#pragma once

#include <string>
#include <random>
#include <SFML/Graphics.hpp>

const unsigned MEMORY_SIZE = 4096;
const unsigned short PROGRAM_MEMORY_OFFSET = 0x200;

const unsigned DISPLAY_WIDTH = 64;
const unsigned DISPLAY_HEIGHT = 32;

class Chip8
{
public:
    Chip8();
    void loadProgram(const std::string& fileName);
    void reset();
    void run();

private:
    void emulateCycle();
    void processOpcode(unsigned short opcode);
    void decrementTimers();
    void draw();
    void handleInput();

private: // opcodes
    void process_00E0(unsigned short opcode);
    void process_00EE(unsigned short opcode);
    void process_1NNN(unsigned short opcode);
    void process_2NNN(unsigned short opcode);
    void process_3XNN(unsigned short opcode);
    void process_4XNN(unsigned short opcode);
    void process_5XY0(unsigned short opcode);
    void process_6XNN(unsigned short opcode);
    void process_7XNN(unsigned short opcode);
    void process_8XY0(unsigned short opcode);
    void process_8XY1(unsigned short opcode);
    void process_8XY2(unsigned short opcode);
    void process_8XY3(unsigned short opcode);
    void process_8XY4(unsigned short opcode);
    void process_8XY5(unsigned short opcode);
    void process_8XY6(unsigned short opcode);
    void process_8XY7(unsigned short opcode);
    void process_8XYE(unsigned short opcode);
    void process_9XY0(unsigned short opcode);
    void process_ANNN(unsigned short opcode);
    void process_BNNN(unsigned short opcode);
    void process_CXNN(unsigned short opcode);
    void process_DXYN(unsigned short opcode);
    void process_EX9E(unsigned short opcode);
    void process_EXA1(unsigned short opcode);
    void process_FX07(unsigned short opcode);
    void process_FX0A(unsigned short opcode);
    void process_FX15(unsigned short opcode);
    void process_FX18(unsigned short opcode);
    void process_FX1E(unsigned short opcode);
    void process_FX29(unsigned short opcode);
    void process_FX33(unsigned short opcode);
    void process_FX55(unsigned short opcode);
    void process_FX65(unsigned short opcode);
    

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

    bool redraw;
    sf::RenderWindow window;
    std::mt19937 engine;
    std::uniform_int_distribution<> distribution;
};

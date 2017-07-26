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
    void process_0000(unsigned short opcode);
    void process_8000(unsigned short opcode);
    void process_E000(unsigned short opcode);
    void process_F000(unsigned short opcode);

    void process_00E0(unsigned short opcode); // 0x00E0: clear the screen
    void process_00EE(unsigned short opcode); // 0x00EE: return from a subroutine
    void process_1NNN(unsigned short opcode); // 0x1NNN: jump to address NNN
    void process_2NNN(unsigned short opcode); // 0x2NNN: execute subrouting starting at address NNN
    void process_3XNN(unsigned short opcode); // 0x3XNN: skip the following instruction if VX == NN
    void process_4XNN(unsigned short opcode); // 0x4XNN: skip the following instruction if VX != NN
    void process_5XY0(unsigned short opcode); // 0x5XY0: skip the following instruction if VX == VY
    void process_6XNN(unsigned short opcode); // 0x6XNN: store NN in VX
    void process_7XNN(unsigned short opcode); // 0x7XNN: add NN to VX
    void process_8XY0(unsigned short opcode); // 0x8XY0: store VY in VX
    void process_8XY1(unsigned short opcode); // 0x8XY1: set VX to VX | VY
    void process_8XY2(unsigned short opcode); // 0x8XY2: set VX to VX & VY
    void process_8XY3(unsigned short opcode); // 0x8XY3: set VX to VX ^ VY
    void process_8XY4(unsigned short opcode); // 0x8XY4: add VY to VX; set VF to 01 if a carry occurs, 00 otherwise
    void process_8XY5(unsigned short opcode); // 0x8XY5: substruct VY from VX; set VF to 00 if a borrow occurs, 01 otherwise
    void process_8XY6(unsigned short opcode); // 0x8XY6: store VY shifted right one bit in VX;
                                              // set VF to the least significant bit prior to the shift
    void process_8XY7(unsigned short opcode); // 0x8XY7: set VX to VY - VX; set VF to 00 if a borrow occurs, 01 otherwise
    void process_8XYE(unsigned short opcode); // 0x8XYE: store VY shifted left one bit in VX;
                                              // set VF to the most significant bit prior to the shift
    void process_9XY0(unsigned short opcode); // 0x9XY0: skip the following instruction if VX != VY
    void process_ANNN(unsigned short opcode); // 0xANNN: store NNN in I
    void process_BNNN(unsigned short opcode); // 0xBNNN: jump to address NNN + V0
    void process_CXNN(unsigned short opcode); // 0xCXNN: set VX to a random number with a mask NN
    void process_DXYN(unsigned short opcode); // 0xDXYN: draw a sprite at position (VX, VY) with N bytes of sprite data starting at the address I; 
                                              // set VF to 01 if any set pixels are changed to unset, and 00 otherwise
    void process_EX9E(unsigned short opcode); // 0xEX9E: skip the following instruction if the key corresponding
                                              // to the hex value currently stored in VX is pressed
    void process_EXA1(unsigned short opcode); // 0xEXA1: skip the following instruction if the key corresponding
                                              // to the hex value currently stored in VX is not pressed
    void process_FX07(unsigned short opcode); // 0xFX07: store the current value of the delay timer in VX
    void process_FX0A(unsigned short opcode); // 0xFX0A: wait for a keypress and store the result in VX
    void process_FX15(unsigned short opcode); // 0xFX15: set the delay timer to the value of VX
    void process_FX18(unsigned short opcode); // 0xFX18: set the sound timer to the value of VX
    void process_FX1E(unsigned short opcode); // 0xFX1E: add the value stored in VX to I
    void process_FX29(unsigned short opcode); // 0xFX29: set I to the memory address of the sprite data corresponding
                                              // to the hexadecimal digit stored in VX
    void process_FX33(unsigned short opcode); // 0xFX33: store the binary-coded decimal equivalent of the value
                                              // stored in VX at addresses I, I + 1, and I + 2
    void process_FX55(unsigned short opcode); // 0xFX55: store the values of registers V0 to VX inclusive in memory starting at address I;
                                              // I is set to I + X + 1 after operation
    void process_FX65(unsigned short opcode); // 0xFX65: fill registers V0 to VX inclusive with the values stored in memory starting at address I
                                              // I is set to I + X + 1 after operation
    

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

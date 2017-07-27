#include <random>

const unsigned MEMORY_SIZE = 4096;
const unsigned short PROGRAM_MEMORY_OFFSET = 0x200;

const unsigned DISPLAY_WIDTH = 64;
const unsigned DISPLAY_HEIGHT = 32;

class CPU
{
public:
    CPU();
    void emulateCycle();
    void decrementTimers();

    bool redraw();
    bool playSound() const;

private:
    void processOpcode(uint16_t opcode);

    void process_00EN(uint16_t opcode);
    void process_8XYN(uint16_t opcode);
    void process_EXNN(uint16_t opcode);
    void process_FXNN(uint16_t opcode);

    void process_00E0(uint16_t opcode); // 0x00E0: clear the screen
    void process_00EE(uint16_t opcode); // 0x00EE: return from a subroutine
    void process_1NNN(uint16_t opcode); // 0x1NNN: jump to address NNN
    void process_2NNN(uint16_t opcode); // 0x2NNN: execute subrouting starting at address NNN
    void process_3XNN(uint16_t opcode); // 0x3XNN: skip the following instruction if VX == NN
    void process_4XNN(uint16_t opcode); // 0x4XNN: skip the following instruction if VX != NN
    void process_5XY0(uint16_t opcode); // 0x5XY0: skip the following instruction if VX == VY
    void process_6XNN(uint16_t opcode); // 0x6XNN: store NN in VX
    void process_7XNN(uint16_t opcode); // 0x7XNN: add NN to VX
    void process_8XY0(uint16_t opcode); // 0x8XY0: store VY in VX
    void process_8XY1(uint16_t opcode); // 0x8XY1: set VX to VX | VY
    void process_8XY2(uint16_t opcode); // 0x8XY2: set VX to VX & VY
    void process_8XY3(uint16_t opcode); // 0x8XY3: set VX to VX ^ VY
    void process_8XY4(uint16_t opcode); // 0x8XY4: add VY to VX; set VF to 01 if a carry occurs, 00 otherwise
    void process_8XY5(uint16_t opcode); // 0x8XY5: substruct VY from VX; set VF to 00 if a borrow occurs, 01 otherwise
    void process_8XY6(uint16_t opcode); // 0x8XY6: store VY shifted right one bit in VX;
                                        // set VF to the least significant bit prior to the shift
    void process_8XY7(uint16_t opcode); // 0x8XY7: set VX to VY - VX; set VF to 00 if a borrow occurs, 01 otherwise
    void process_8XYE(uint16_t opcode); // 0x8XYE: store VY shifted left one bit in VX;
                                        // set VF to the most significant bit prior to the shift
    void process_9XY0(uint16_t opcode); // 0x9XY0: skip the following instruction if VX != VY
    void process_ANNN(uint16_t opcode); // 0xANNN: store NNN in I
    void process_BNNN(uint16_t opcode); // 0xBNNN: jump to address NNN + V0
    void process_CXNN(uint16_t opcode); // 0xCXNN: set VX to a random number with a mask NN
    void process_DXYN(uint16_t opcode); // 0xDXYN: draw a sprite at position (VX, VY) with N bytes of sprite data starting at the address I; 
                                        // set VF to 01 if any set pixels are changed to unset, and 00 otherwise
    void process_EX9E(uint16_t opcode); // 0xEX9E: skip the following instruction if the key corresponding
                                        // to the hex value currently stored in VX is pressed
    void process_EXA1(uint16_t opcode); // 0xEXA1: skip the following instruction if the key corresponding
                                        // to the hex value currently stored in VX is not pressed
    void process_FX07(uint16_t opcode); // 0xFX07: store the current value of the delay timer in VX
    void process_FX0A(uint16_t opcode); // 0xFX0A: wait for a keypress and store the result in VX
    void process_FX15(uint16_t opcode); // 0xFX15: set the delay timer to the value of VX
    void process_FX18(uint16_t opcode); // 0xFX18: set the sound timer to the value of VX
    void process_FX1E(uint16_t opcode); // 0xFX1E: add the value stored in VX to I
    void process_FX29(uint16_t opcode); // 0xFX29: set I to the memory address of the sprite data corresponding
                                        // to the hexadecimal digit stored in VX
    void process_FX33(uint16_t opcode); // 0xFX33: store the binary-coded decimal equivalent of the value
                                        // stored in VX at addresses I, I + 1, and I + 2
    void process_FX55(uint16_t opcode); // 0xFX55: store the values of registers V0 to VX inclusive in memory starting at address I;
                                        // I is set to I + X + 1 after operation
    void process_FX65(uint16_t opcode); // 0xFX65: fill registers V0 to VX inclusive with the values stored in memory starting at address I
                                        // I is set to I + X + 1 after operation

public:
    uint8_t memory[MEMORY_SIZE];
    uint8_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    uint8_t keypad[16];

private:
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;
    uint16_t stack[16];
    uint8_t sp;

    uint8_t delayTimer;
    uint8_t soundTimer;
    bool drawFlag;

    std::mt19937 engine;
    std::uniform_int_distribution<> distribution;
};

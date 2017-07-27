#include "CPU.hpp"

#include <array>
#include <map>
#include <functional>

CPU::CPU() :
    engine{ std::random_device()() },
    distribution{ 0, 0xFF }
{
    pc = PROGRAM_MEMORY_OFFSET;
    sp = 0;
    I = 0;

    std::fill(std::begin(memory), std::end(memory), 0);
    std::fill(std::begin(V), std::end(V), 0);
    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(display), std::end(display), 0);
    std::fill(std::begin(keypad), std::end(keypad), 0);

    delayTimer = 0;
    soundTimer = 0;
    drawFlag = false;

    const int FONTSET_SIZE = 80;
    unsigned char fontset[FONTSET_SIZE] =
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

    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[i] = fontset[i];
    }
}

void CPU::emulateCycle()
{
    // fetch opcode
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    // process opcode
    processOpcode(opcode);
}

void CPU::decrementTimers()
{
    if (delayTimer > 0)
    {
        --delayTimer;
    }

    if (soundTimer > 0)
    {
        --soundTimer;
    }
}

bool CPU::redraw()
{
    bool old = drawFlag;
    drawFlag = false;
    return old;
}

bool CPU::playSound() const
{
    return (soundTimer == 1);
}

void CPU::processOpcode(uint16_t opcode)
{
    static const std::array<std::function<void()>, 16> opcodeFunctions =
    {
        [&]() { process_00EN(opcode); },
        [&]() { process_1NNN(opcode); },
        [&]() { process_2NNN(opcode); },
        [&]() { process_3XNN(opcode); },
        [&]() { process_4XNN(opcode); },
        [&]() { process_5XY0(opcode); },
        [&]() { process_6XNN(opcode); },
        [&]() { process_7XNN(opcode); },
        [&]() { process_8XYN(opcode); },
        [&]() { process_9XY0(opcode); },
        [&]() { process_ANNN(opcode); },
        [&]() { process_BNNN(opcode); },
        [&]() { process_CXNN(opcode); },
        [&]() { process_DXYN(opcode); },
        [&]() { process_EXNN(opcode); },
        [&]() { process_FXNN(opcode); }
    };

    uint8_t nibble = (opcode & 0xF000) >> 12;
    opcodeFunctions.at(nibble)();
}

void CPU::process_00EN(uint16_t opcode)
{
    uint8_t nibble = opcode & 0x000F;
    static const std::map<uint8_t, std::function<void()>> opcodeFunctions =
    {
        { 0x0, [&]() { process_00E0(opcode); } },
        { 0xE, [&]() { process_00EE(opcode); } }
    };
    opcodeFunctions.at(nibble)();
}

void CPU::process_8XYN(uint16_t opcode)
{
    uint8_t nibble = opcode & 0x000F;
    static const std::map<uint8_t, std::function<void()>> opcodeFunctions =
    {
        { 0x0, [&]() { process_8XY0(opcode); } },
        { 0x1, [&]() { process_8XY1(opcode); } },
        { 0x2, [&]() { process_8XY2(opcode); } },
        { 0x3, [&]() { process_8XY3(opcode); } },
        { 0x4, [&]() { process_8XY4(opcode); } },
        { 0x5, [&]() { process_8XY5(opcode); } },
        { 0x6, [&]() { process_8XY6(opcode); } },
        { 0x7, [&]() { process_8XY7(opcode); } },
        { 0xE, [&]() { process_8XYE(opcode); } }
    };
    opcodeFunctions.at(nibble)();
}

void CPU::process_EXNN(uint16_t opcode)
{
    uint8_t byte = opcode & 0x00FF;
    static const std::map<uint8_t, std::function<void()>> opcodeFunctions =
    {
        { 0x9E, [&]() { process_EX9E(opcode); } },
        { 0xA1, [&]() { process_EXA1(opcode); } }
    };
    opcodeFunctions.at(byte)();
}

void CPU::process_FXNN(uint16_t opcode)
{
    uint8_t byte = opcode & 0x00FF;
    static const std::map<uint8_t, std::function<void()>> opcodeFunctions =
    {
        { 0x07, [&]() { process_FX07(opcode); } },
        { 0x0A, [&]() { process_FX0A(opcode); } },
        { 0x15, [&]() { process_FX15(opcode); } },
        { 0x18, [&]() { process_FX18(opcode); } },
        { 0x1E, [&]() { process_FX1E(opcode); } },
        { 0x29, [&]() { process_FX29(opcode); } },
        { 0x33, [&]() { process_FX33(opcode); } },
        { 0x55, [&]() { process_FX55(opcode); } },
        { 0x65, [&]() { process_FX65(opcode); } }
    };
    opcodeFunctions.at(byte)();
}

void CPU::process_00E0(uint16_t opcode)
{
    std::fill(std::begin(display), std::end(display), 0);
    drawFlag = true;
    pc += 2;
}

void CPU::process_00EE(uint16_t opcode)
{
    --sp;
    pc = stack[sp];
    pc += 2;
}

void CPU::process_1NNN(uint16_t opcode)
{
    pc = opcode & 0x0FFF;
}

void CPU::process_2NNN(uint16_t opcode)
{
    stack[sp] = pc;
    ++sp;
    pc = opcode & 0x0FFF;
}

void CPU::process_3XNN(uint16_t opcode)
{
    if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}

void CPU::process_4XNN(uint16_t opcode)
{
    if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}

void CPU::process_5XY0(uint16_t opcode)
{
    if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}

void CPU::process_6XNN(uint16_t opcode)
{
    V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
    pc += 2;
}

void CPU::process_7XNN(uint16_t opcode)
{
    V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
    pc += 2;
}

void CPU::process_8XY0(uint16_t opcode)
{
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void CPU::process_8XY1(uint16_t opcode)
{
    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void CPU::process_8XY2(uint16_t opcode)
{
    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void CPU::process_8XY3(uint16_t opcode)
{
    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void CPU::process_8XY4(uint16_t opcode)
{
    if (V[(opcode & 0x0F00) >> 8] > 0xFF - V[(opcode & 0x00F0) >> 4])
    {
        V[0xF] = 0x01;
    }
    else
    {
        V[0xF] = 0x00;
    }
    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void CPU::process_8XY5(uint16_t opcode)
{
    if (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])
    {
        V[0xF] = 0x00;
    }
    else
    {
        V[0xF] = 0x01;
    }
    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}

void CPU::process_8XY6(uint16_t opcode)
{
    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] >> 1;
    pc += 2;
}

void CPU::process_8XY7(uint16_t opcode)
{
    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
    {
        V[0xF] = 0x00;
    }
    else
    {
        V[0xF] = 0x01;
    }
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

void CPU::process_8XYE(uint16_t opcode)
{
    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] << 1;
    pc += 2;
}

void CPU::process_9XY0(uint16_t opcode)
{
    if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}

void CPU::process_ANNN(uint16_t opcode)
{
    I = opcode & 0x0FFF;
    pc += 2;
}

void CPU::process_BNNN(uint16_t opcode)
{
    pc = (opcode & 0x0FFF) + V[0x0];
}

void CPU::process_CXNN(uint16_t opcode)
{
    V[(opcode & 0x0F00) >> 8] = distribution(engine) & (opcode & 0x00FF);
    pc += 2;
}

void CPU::process_DXYN(uint16_t opcode)
{
    uint16_t x = V[(opcode & 0x0F00) >> 8];
    uint16_t y = V[(opcode & 0x00F0) >> 4];
    uint16_t height = opcode & 0x000F;
    uint16_t line;

    V[0xF] = 0x00;
    for (int yline = 0; yline < height; ++yline)
    {
        line = memory[I + yline];
        for (int xline = 0; xline < 8; ++xline)
        {
            if (line & (0x80 >> xline)) // 0x80 -> 0b1000 0000
            {
                if (display[x + xline + (y + yline) * DISPLAY_WIDTH] == 1)
                {
                    V[0xF] = 0x01;
                }

                display[x + xline + (y + yline) * DISPLAY_WIDTH] ^= 1;
            }
        }
    }

    drawFlag = true;
    pc += 2;
}

void CPU::process_EX9E(uint16_t opcode)
{
    if (keypad[V[(opcode & 0x0F00) >> 8]])
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}

void CPU::process_EXA1(uint16_t opcode)
{
    if (!keypad[V[(opcode & 0x0F00) >> 8]])
    {
        pc += 4;
    }
    else
    {
        pc += 2;
    }
}
void CPU::process_FX07(uint16_t opcode)
{
    V[(opcode & 0x0F00) >> 8] = delayTimer;
    pc += 2;
}

void CPU::process_FX0A(uint16_t opcode)
{
    for (int i = 0; i < 16; ++i)
    {
        if (keypad[i])
        {
            V[(opcode & 0x0F00) >> 8] = i;
            pc += 2; // move program counter only if key is pressed
        }
    }
}

void CPU::process_FX15(uint16_t opcode)
{
    delayTimer = V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

void CPU::process_FX18(uint16_t opcode)
{
    soundTimer = V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

void CPU::process_FX1E(uint16_t opcode)
{
    I += V[(opcode & 0x0F00) >> 8];
    pc += 2;
}

void CPU::process_FX29(uint16_t opcode)
{
    const unsigned short FONT_WIDTH = 5;
    I = V[(opcode & 0x0F00) >> 8] * FONT_WIDTH;
    pc += 2;
}

void CPU::process_FX33(uint16_t opcode)
{
    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] % 100) / 10;
    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
    pc += 2;
}

void CPU::process_FX55(uint16_t opcode)
{
    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
    {
        memory[I + i] = V[i];
    }
    I += ((opcode & 0x0F00) >> 8) + 1;
    pc += 2;
}

void CPU::process_FX65(uint16_t opcode)
{
    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
    {
        V[i] = memory[I + i];
    }
    I += ((opcode & 0x0F00) >> 8) + 1;
    pc += 2;
}

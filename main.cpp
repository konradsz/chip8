#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <random>
#include <SFML/Graphics.hpp>

const unsigned DISPLAY_WIDTH = 64;
const unsigned DISPLAY_HEIGHT = 32;
const unsigned MEMORY_SIZE = 4096;
const unsigned short PROGRAM_MEMORY_OFFSET = 0x200;

const float TILE_SIZE = 10.0f;

//char display[DISPLAY_WIDTH * DISPLAY_HEIGHT]{};

/*void setPixel(int x, int y)
{
    display[x + y * DISPLAY_WIDTH] = 1;
}*/

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

class Chip8
{
public:
    Chip8();
    void loadProgram(const std::string& fileName);
    void initialize();
    void emulateCycle();
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

Chip8::Chip8() :
    engine{ std::random_device()() },
    distribution{ 0, 0xFF }
{
}

void Chip8::loadProgram(const std::string& fileName)
{
    std::ifstream program(fileName.c_str(), std::ios::binary);
    if (program.is_open())
    {
        std::vector<char> buffer{
            std::istreambuf_iterator<char>(program),
            std::istreambuf_iterator<char>() };
        if (buffer.size() <= MEMORY_SIZE - PROGRAM_MEMORY_OFFSET)
        {
            std::copy(std::begin(buffer), std::end(buffer),
                std::begin(memory) + PROGRAM_MEMORY_OFFSET);
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

void Chip8::initialize()
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
}

void Chip8::emulateCycle()
{
    // fetch opcode
    unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
    // process opcode
    processOpcode(opcode);
    // decrement timers
    if (delayTimer > 0)
        --delayTimer;

    if (soundTimer > 0)
    {
        if (soundTimer == 1)
            std::cout << "BEEP" << std::endl; // playSound!
        --soundTimer;
    }
}

void Chip8::processOpcode(unsigned short opcode)
{
    // what about getting each hex separately? (NIBBLE - 4 bits)
    // first = opcode & 0xF000
    // second = opcode & 0x0F00
    // third = opcode & 0x00F0
    // fourth = opcode & 0x000F
    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        case 0x00E0: // 0x00E0: clear the screen
            std::fill(std::begin(display), std::end(display), 0);
            drawFlag = true;
            pc += 2;
            break;
        case 0x00EE: // 0x00EE: return from a subroutine
            --sp;
            pc = stack[sp];
            pc += 2;
            break;
        }
        break;
    case 0x1000: // 0x1NNN: jump to address NNN
        pc = opcode & 0x0FFF;
        break;
    case 0x2000: // 0x2NNN: execute subrouting starting at address NNN
        stack[sp] = pc;
        ++sp;
        pc = opcode & 0x0FFF;
        break;
    case 0x3000: // 0x3XNN: skip the following instruction if VX == NN
        if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    case 0x4000: // 0x4XNN: skip the following instruction if VX != NN
        if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    case 0x5000: // 0x5XY0: skip the following instruction if VX == VY
        if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    case 0x6000: // 0x6XNN: store NN in VX
        V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        pc += 2;
        break;
    case 0x7000: // 0x7XNN: add NN to VX
        V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        pc += 2;
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 0x8XY0: store VY in VX
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0001: // 0x8XY1: set VX to VX | VY
            V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0002: // 0x8XY2: set VX to VX & VY
            V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0003: // 0x8XY3: set VX to VX ^ VY
            V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;
        case 0x0004: // 0x8XY4: add VY to VX; set VF to 01 if a carry occurs, 00 otherwise
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
            break;
        case 0x0005: // 0x8XY5: substruct VY from VX; set VF to 00 if a borrow occurs, 01 otherwise
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
            break;
        case 0x0006: // 0x8XY6: store VY shifted right one bit in VX;
                     // set VF to the least significant bit prior to the shift
            V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] >> 1;
            pc += 2;
            break;
        case 0x0007: // 0x8XY7: set VX to VY - VX; set VF to 00 if a borrow occurs, 01 otherwise
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
            break;
        case 0x000E: // 0x8XYE: store VY shifted left one bit in VX;
                     // set VF to the most significant bit prior to the shift
            V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] << 1;
            pc += 2;
            break;
        }
        break;
    case 0x9000: // 0x9XY0: skip the following instruction if VX != VY
        if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
        {
            pc += 4;
        }
        else
        {
            pc += 2;
        }
        break;
    case 0xA000: // 0xANNN: store NNN in I
        I = opcode & 0x0FFF;
        pc += 2;
        break;
    case 0xB000: // 0xBNNN: jump to address NNN + V0
        pc = (opcode & 0x0FFF) + V[0x0];
        break;
    case 0xC000: // 0xCXNN: set VX to a random number with a mask NN
        V[(opcode & 0x0F00) >> 8] = distribution(engine) & (opcode & 0x00FF);
        pc += 2;
        break;
    case 0xD000: // 0xDXYN: draw a sprite at position (VX, VY) with N bytes of sprite data starting at the address I; 
                 // set VF to 01 if any set pixels are changed to unset, and 00 otherwise
        {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short line;

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
        break;
    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E: // 0xEX9E: skip the following instruction if the key corresponding
                     // to the hex value currently stored in VX is pressed
            if (keypad[V[(opcode & 0x0F00) >> 8]])
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        case 0x00A1: // 0xEXA1: skip the following instruction if the key corresponding
                        // to the hex value currently stored in VX is not pressed
            if (!keypad[V[(opcode & 0x0F00) >> 8]])
            {
                pc += 4;
            }
            else
            {
                pc += 2;
            }
            break;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007: // 0xFX07: store the current value of the delay timer in VX
            V[(opcode & 0x0F00) >> 8] = delayTimer;
            pc += 2;
            break;
        case 0x000A: // 0xFX0A: wait for a keypress and store the result in VX
            for (int i = 0; i < 16; ++i)
            {
                if (keypad[i])
                {
                    V[(opcode & 0x0F00) >> 8] = i;
                    pc += 2; // move program counter only if key is pressed
                }
            }
            break;
        case 0x0015: // 0xFX15: set the delay timer to the value of VX
            delayTimer = V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x0018: // 0xFX18: set the sound timer to the value of VX
            soundTimer = V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x001E: // 0xFX1E: add the value stored in VX to I
            I += V[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;
        case 0x0029: // 0xFX29: set I to the memory address of the sprite data corresponding
                     // to the hexadecimal digit stored in VX
        {
            const unsigned short FONT_WIDTH = 5;
            I = V[(opcode & 0x0F00) >> 8] * FONT_WIDTH;
            pc += 2;
            break;
        }
        case 0x0033: // 0xFX33: store the binary-coded decimal equivalent of the value
                     // stored in VX at addresses I, I + 1, and I + 2
            memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] % 100) / 10;
            memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
            pc += 2;
            break;
        case 0x0055: // 0xFX55: store the values of registers V0 to VX inclusive in memory starting at address I;
                     // I is set to I + X + 1 after operation
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
            {
                memory[I + i] = V[i];
            }
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;
        case 0x0065: // 0xFX65: fill registers V0 to VX inclusive with the values stored in memory starting at address I
                     // I is set to I + X + 1 after operation
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
            {
                V[i] = memory[I + i];
            }
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;
        }
        break;
    }
}

bool Chip8::draw()
{
    if (drawFlag)
    {
        drawFlag != drawFlag;
        return true;
    }
    return false;
}

unsigned char* Chip8::getDisplay()
{
    return display;
}

void Chip8::setKeyState(unsigned short keyCode, sf::Event::EventType eventType)
{
    if (eventType == sf::Event::KeyPressed)
    {
        keypad[keyCode] = 1;
    }
    else if (eventType == sf::Event::KeyReleased)
    {
        keypad[keyCode] = 0;
    }
}

int main()
{
    Chip8 chip;
    chip.initialize();
    //chip.loadProgram("mathMaze.ch8");
    //chip.loadProgram("invaders.c8");
    chip.loadProgram("tetris.c8");

    sf::RenderWindow window(sf::VideoMode(DISPLAY_WIDTH * TILE_SIZE, DISPLAY_HEIGHT * TILE_SIZE), "Chip8");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

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

        chip.emulateCycle();

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

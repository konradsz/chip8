#include <iostream>

#include "Chip8.hpp"

int main(int argc, char* argv[])
{
    Chip8 chip;

    if (argc != 2)
    {
        std::cerr << "Usage: ./" << argv[0] << " pathToROM" << std::endl;
        return 0;
    }

    if (chip.loadROM(argv[1]))
    {
        chip.run();
    }

    return 0;
}

#include "Chip8.hpp"

int main()
{
    Chip8 chip;

    //chip.loadProgram("mathMaze.ch8");
    //chip.loadProgram("invaders.c8");
    //chip.loadProgram("tetris.c8");
    chip.loadROM("../roms/BOWLING");

    chip.run();

    return 0;
}

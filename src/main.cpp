#include "Chip8.hpp"

int main()
{
    Chip8 chip;

    //chip.loadProgram("mathMaze.ch8");
    //chip.loadProgram("invaders.c8");
    //chip.loadProgram("tetris.c8");
    chip.loadProgram("../roms/R-ROULETTE");

    chip.run();

    return 0;
}

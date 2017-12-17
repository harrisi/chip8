#include <stdio.h>
#include <stdbool.h>
#include "chip8.h"
#include <ncurses.h>

chip8 myChip8;

int main(int argc, char *argv[]) {
    // setupGraphics();
    // setupInput();

    initialize(&myChip8);
    loadgame(&myChip8, "pong.rom");

    int i = 0;
    //while (i < 10) {
    while (myChip8.pc < 4096) {
        emulatecycle(&myChip8);

        // if (myChip8.drawFlag) {
        //     drawGraphics(&myChip8);
        // }

        // setKeys(&myChip8);
        i++;
    }

    return 0;
}

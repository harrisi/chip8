#include <stdio.h>
#include <stdbool.h>
//#include <time.h>
#include <ncurses.h>
#include "chip8.h"
#include "disp.h"

chip8 myChip8;

int main(int argc, char *argv[]) {
    setupGraphics();
    setupInput();

    initialize(&myChip8);
    loadgame(&myChip8, "pong.rom");

    int i = 0;
    //while (i < 10) {
    while (myChip8.pc < 4096) {
        emulatecycle(&myChip8);

        if (myChip8.drawFlag) {
            drawGraphics(&myChip8);
        }

        setKeys(&myChip8, getch());
        i++;
        // nanosleep((const struct timespec[]){{0, 1000000L}}, NULL);
    }

    refresh();
    endwin();

    return 0;
}

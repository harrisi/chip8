#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include "chip8.h"
#include "disp.h"

chip8 myChip8;

int main(int argc, char *argv[]) {
    char *game = "pong.rom";

    if (argc == 2) {
        game = malloc(strlen(argv[1]) + 1);
        strcpy(game, argv[1]);
    }

    setupGraphics();
    setupInput();

    initialize(&myChip8);

    loadgame(&myChip8, game);

    int i = 0;
    while (true) {
        emulatecycle(&myChip8);

        if (myChip8.drawFlag) {
            drawGraphics(&myChip8);
        }

        setKeys(&myChip8, 666);
        i++;
        nanosleep((const struct timespec[]){{0, 2000000L}}, NULL);
    }

    refresh();
    endwin();

    return 0;
}

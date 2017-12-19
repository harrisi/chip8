#include "disp.h"
#include "chip8.h"
#include <string.h>
#include <stdlib.h>

void setupGraphics() {
    initscr();
    noecho();
    cbreak();
    //halfdelay(1);
    curs_set(0);
    keypad(stdscr, true);
}

void setupInput() {
    // nothing to do here
}

void drawGraphics(chip8 *chip) {
    char s[32][64] = {{0}};
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
            //if chip->gfx[i + j]
        }
    }
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 64; j++) {
            mvaddch(i, j, chip->gfx[i * 64 + j] == 0 ? ' ' : '*');
        }
    }

    refresh();
    chip->drawFlag = false;
}

void setKeys(chip8 *chip, int ch) {
    if (ch == ERR) {
        memset(chip->keys, 0, sizeof(chip->keys));
    }
    else if (ch == 27) {
        endwin();
        exit(0);
    }
    else if (ch == '1') chip->keys[0x1] = 1;
    else if (ch == '2') chip->keys[0x2] = 1;
    else if (ch == '3') chip->keys[0x3] = 1;
    else if (ch == '4') chip->keys[0xC] = 1;

    else if (ch == 'q') chip->keys[0x4] = 1;
    else if (ch == 'w') chip->keys[0x5] = 1;
    else if (ch == 'e') chip->keys[0x6] = 1;
    else if (ch == 'r') chip->keys[0xD] = 1;

    else if (ch == 'a') chip->keys[0x7] = 1;
    else if (ch == 's') chip->keys[0x8] = 1;
    else if (ch == 'd') chip->keys[0x9] = 1;
    else if (ch == 'f') chip->keys[0xE] = 1;

    else if (ch == 'z') chip->keys[0xA] = 1;
    else if (ch == 'x') chip->keys[0x0] = 1;
    else if (ch == 'c') chip->keys[0xB] = 1;
    else if (ch == 'v') chip->keys[0xF] = 1;
}

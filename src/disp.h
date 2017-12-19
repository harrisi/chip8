#ifndef _DISP_H_
#define _DISP_H_

#include "chip8.h"
#include <ncurses.h>

void setupGraphics();
void setupInput();
void drawGraphics(chip8 *);
void setKeys(chip8 *, int);

#endif

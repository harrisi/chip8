#include <stdlib.h> // exit
#include <string.h> // memcpy
#include <stdint.h>
#include <stdio.h>
#include "chip8.h"
#include <time.h>

void initialize(chip8 *chip) {
    // I'm not sure what to set these to because the values are allocated on the
    // stack anyway.
    chip->pc = 0x200;
    chip->opcode = 0;
    chip->I = 0;
    chip->sp = 0;

    static const uint8_t keys[16] = {
        '1', '2', '3', '4',
        'q', 'w', 'e', 'r',
        'a', 's', 'd', 'f',
        'z', 'x', 'c', 'v'
    };

    memcpy(chip->keys, keys, sizeof(keys));

    uint8_t chip8_fontset[80] = {
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

    // clear display
    // clear stack
    // clear registers V0-VF
    // clear memory

    // load fontset
    for (int i = 0; i < 80; i++) {
        chip->memory[i] = chip8_fontset[i];
    }

    // reset timers
    chip->delay_timer = 0;
    chip->sound_timer = 0;

    srand(time(NULL));
}

void loadgame(chip8 *chip, char *filename) {
    FILE *f = fopen(filename, "rb"); // according to `man fopen`, 'b' isn't needed.

    if (!f) {
        fprintf(stderr, "ERROR: Couldn't open file %s\n", filename);
        exit(1);
    }
    
    int i = 0, c; // for EOF
    while ((c = fgetc(f)) != EOF) {
        chip->memory[i + 512] = c;
        i++;
    }

    fclose(f);
}

void emulatecycle(chip8 *chip) {
    // fetch
    chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

    // decode
    operation op = decode(chip->opcode);

    if (!op) {
        fprintf(stderr, "UNKNOWN OPCODE: %d\n", chip->opcode);
        exit(2);
    }

    // execute

    (*op)(chip);

    // update timers

    if (chip->delay_timer > 0) {
        chip->delay_timer--;
    }
    if (chip->sound_timer > 0) {
        if (chip->sound_timer == 1) {
            printf("\a");
        }
        chip->sound_timer--;
    }

}

operation decode(uint16_t opcode) {
    // opcodes without variables
    switch (opcode) {
        case 0x00E0:
            return (operation)disp_clear;
        case 0x00EE:
            return (operation)return_fn;
    }

    // opcodes with variables
    switch ((opcode & 0xF000) >> 12) {
        case 0x0:
            return (operation)call_rca1802;
        case 0x1:
            return (operation)goto_fn;
        case 0x2:
            return (operation)call_subr;
        case 0x3:
            return (operation)if_reg_eq_const;
        case 0x4:
            return (operation)if_reg_neq_const;
        case 0x5:
            return (operation)if_reg_eq_reg;
        case 0x6:
            return (operation)set_reg_const;
        case 0x7:
            return (operation)add_const_reg;
        case 0x8:

            switch (opcode & 0x000F) {
                case 0x0:
                    return (operation)assign_reg_reg;
                case 0x1:
                    return (operation)assign_or_reg_reg;
                case 0x2:
                    return (operation)assign_and_reg_reg;
                case 0x3:
                    return (operation)assign_xor_reg_reg;
                case 0x4:
                    return (operation)add_reg_reg;
                case 0x5:
                    return (operation)sub_reg_reg;
                case 0x6:
                    return (operation)shr_reg_reg;
                case 0x7:
                    return (operation)sub_reg_reg_ex;
                case 0xE:
                    return (operation)shl_reg_reg;
            }

        case 0x9:
            return (operation)if_reg_neq_reg;
        case 0xA:
            return (operation)set_i_const;
        case 0xB:
            return (operation)set_pc_v0_const;
        case 0xC:
            return (operation)set_reg_rand_and_const;
        case 0xD:
            return (operation)draw_reg_reg_height_const;
        case 0xE:
            if ((opcode & 0x00FF) == 0x009E) {
                return (operation)if_key_eq_reg;
            } else { // opcode & 0x00FF == 0x00A1
                return (operation)if_key_neq_reg;
            }
        case 0xF:

            switch (opcode & 0x00F0) {
                case 0x00:
                    if ((opcode & 0x000F) == 0x0007) {
                        return (operation)set_reg_delay_timer;
                    } else if ((opcode & 0x000F) == 0x000A) {
                        return (operation)wait_set_key_reg;
                    }
                case 0x10:
                    switch (opcode & 0xF) {
                        case 0x5:
                            return (operation)set_delay_timer_reg;
                        case 0x8:
                            return (operation)set_sound_timer_reg;
                        case 0xE:
                            return (operation)i_add_assign_reg;
                    }
                case 0x20:
                    return (operation)set_i_sprite_reg;
                case 0x30:
                    return (operation)set_i_bcd;
                case 0x50:
                    return (operation)reg_dump;
                case 0x60:
                    return (operation)reg_load;
            }
    }
    return NULL; // to get rid of warning
    // hopefully this never happens, otherwise that means we got to an unknown
    // opcode.
}

void disp_clear(chip8 *chip) {
    memset(chip->gfx, 0, sizeof(chip->gfx));
    chip->drawFlag = true;
    chip->pc += 2;
}

void return_fn(chip8 *chip) {
    chip->sp--;
    chip->pc = chip->stack[chip->sp];
    chip->pc += 2;
}

void call_rca1802(chip8 *chip) {
    // apparently not needed?
}

void goto_fn(chip8 *chip) {
    chip->pc = chip->opcode & 0x0FFF;
}

void call_subr(chip8 *chip) {
    chip->stack[chip->sp] = chip->pc;
    chip->sp++;
    chip->pc = chip->opcode & 0x0FFF;
}

void if_reg_eq_const(chip8 *chip) {
    uint8_t const_val = chip->opcode & 0x00FF;
    if (chip->V[(chip->opcode & 0x0F00) >> 8] == const_val) {
        chip->pc += 4;
    } else {
        chip->pc += 2;
    }
}

void if_reg_neq_const(chip8 *chip) {
    uint8_t const_val = chip->opcode & 0x00FF;
    if (chip->V[(chip->opcode & 0x0F00) >> 8] != const_val) {
        chip->pc += 4;
    } else {
        chip->pc += 2;
    }
}

void if_reg_eq_reg(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x0F00) >> 8] ==
            chip->V[(chip->opcode & 0x00F0) >> 4]) {
        chip->pc += 4;
    } else {
        chip->pc += 2;
    }
}

void set_reg_const(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] = (chip->opcode & 0x00FF);
    chip->pc += 2;
}

void add_const_reg(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] += (chip->opcode & 0x00FF);
    chip->pc += 2;
}

void assign_reg_reg(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] =
        chip->V[(chip->opcode & 0x00F0) >> 4];
    chip->pc += 2;
}

void assign_or_reg_reg(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] |=
        chip->V[(chip->opcode & 0x00F0) >> 4];
    chip->pc += 2;
}

void assign_and_reg_reg(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] &=
        chip->V[(chip->opcode & 0x00F0) >> 4];
    chip->pc += 2;
}

void assign_xor_reg_reg(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] ^=
        chip->V[(chip->opcode & 0x00F0) >> 4];
    chip->pc += 2;
}

void add_reg_reg(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x00F0) >> 4] >
            (0xFF - chip->V[(chip->opcode & 0xF00) >> 8])) {
        chip->V[0xF] = 1; // carry
    } else {
        chip->V[0xF] = 0;
    }
    chip->V[(chip->opcode & 0x0F00) >> 8] +=
        chip->V[(chip->opcode & 0x00F0) >> 4];
    chip->pc += 2;
}

void sub_reg_reg(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x0F00) >> 8] >
            chip->V[(chip->opcode & 0x00F0) >> 4]) {
        chip->V[0xF] = 1;
    } else {
        chip->V[0xF] = 0;
    }
    chip->V[(chip->opcode & 0x0F00) >> 8] -=
        chip->V[(chip->opcode & 0x00F0) >> 4];
    chip->pc += 2;
}

void shr_reg_reg(chip8 *chip) {
    chip->V[0xF] = chip->V[(chip->opcode & 0x0F00) >> 8] & 0x1;
    chip->V[(chip->opcode & 0x0F00) >> 8] >>= 1;
    chip->pc += 2;
}

void sub_reg_reg_ex(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x00F0) >> 4] >
            chip->V[(chip->opcode & 0x0F00) >> 8]) {
        chip->V[0xF] = 1;
    } else {
        chip->V[0xF] = 0;
    }
    chip->V[(chip->opcode & 0x0F00) >> 8] =
        chip->V[(chip->opcode & 0x00F0) >> 4] -
        chip->V[(chip->opcode & 0x0F00) >> 8];
    chip->pc += 2;
}

void shl_reg_reg(chip8 *chip) {
    chip->V[0xF] = chip->V[(chip->opcode & 0x0F00) >> 8] >> 7;
    chip->V[(chip->opcode & 0x0F00) >> 8] <<= 1;
    chip->pc += 2;
}

void if_reg_neq_reg(chip8 *chip) {
    if (chip->V[(chip->opcode & 0x0F00) >> 8] !=
            chip->V[(chip->opcode & 0x00F0) >> 4]) {
        chip->pc += 4;
    } else {
        chip->pc += 2;
    }
}

void set_i_const(chip8 *chip) {
    chip->I = (chip->opcode & 0x0FFF);
    chip->pc += 2;
}

void set_pc_v0_const(chip8 *chip) {
    chip->pc = (chip->opcode & 0x0FFF) + chip->V[0];
}

// XXX: This might not be right?
void set_reg_rand_and_const(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] = (rand() % (chip->opcode & 0x00FF));
    chip->pc += 2;
}

void draw_reg_reg_height_const(chip8 *chip) {
    uint16_t x = chip->V[(chip->opcode & 0x0F00) >> 8];
    uint16_t y = chip->V[(chip->opcode & 0x00F0) >> 4];
    uint16_t height = chip->opcode & 0x000F;
    uint16_t pixel;

    chip->V[0xF] = 0; // set detection flag to 0
    for (int yline = 0; yline < height; yline++) {
        pixel = chip->memory[chip->I + yline];
        for (int xline = 0; xline < 8; xline++) {
            if ((pixel & (0x80 >> xline)) != 0) {
                if (chip->gfx[(x + xline + ((y + yline) * 64))] == 1) {
                    chip->V[0xF] = 1; // collision detected
                }
                chip->gfx[x + xline + ((y + yline) * 64)] ^= 1;
            }
        }
    }

    chip->drawFlag = true;
    chip->pc += 2;
}

void if_key_eq_reg(chip8 *chip) {
    if (chip->keys[chip->V[(chip->opcode & 0x0F00) >> 8]] == 1) {
        chip->pc += 4; // skip next instruction if key stored in VX is pressed
    } else {
        chip->pc += 2;
    }
}

void if_key_neq_reg(chip8 *chip) {
    if (chip->keys[chip->V[(chip->opcode & 0x0F00) >> 8]] == 0) {
        chip->pc += 4;
    } else {
        chip->pc += 2;
    }
}

void set_reg_delay_timer(chip8 *chip) {
    chip->V[(chip->opcode & 0x0F00) >> 8] = chip->delay_timer;
    chip->pc += 2;
}

void wait_set_key_reg(chip8 *chip) {
    bool keypress = false;

    for (int i = 0; i < 16; i++) {
        if (chip->keys[i] != 0) {
            chip->V[(chip->opcode & 0x0F00) >> 8] = i;
            keypress = true;
        }
    }

    if (!keypress) {
        return;
    }

    //chip->V[(chip->opcode & 0x0F00) >> 8] = getchar();
    chip->pc += 2;
}

void set_delay_timer_reg(chip8 *chip) {
    chip->delay_timer = chip->V[(chip->opcode & 0x0F00) >> 8];
    chip->pc += 2;
}

void set_sound_timer_reg(chip8 *chip) {
    chip->sound_timer = chip->V[(chip->opcode & 0x0F00) >> 8];
    chip->pc += 2;
}

void i_add_assign_reg(chip8 *chip) {
    if (chip->I + chip->V[(chip->opcode & 0x0F00) >> 8] > 0xFFF) {
        chip->V[0xF] = 1;
    } else {
        chip->V[0xF] = 0;
    }
    chip->I += chip->V[(chip->opcode & 0x0F00) >> 8];
    chip->pc += 2;
}

void set_i_sprite_reg(chip8 *chip) {
    // XXX: this is definitely wrong
    chip->I = chip->V[(chip->opcode & 0x0F00) >> 8] * 0x5;
    // XXX: why `* 0x5`?
    chip->pc += 2;
}

void set_i_bcd(chip8 *chip) {
    chip->memory[chip->I] = chip->V[(chip->opcode & 0x0F00) >> 8] / 100;
    chip->memory[chip->I + 1] = (chip->V[(chip->opcode & 0x0F00) >> 8] / 10) % 10;
    chip->memory[chip->I + 2] = (chip->V[(chip->opcode & 0x0F00) >> 8] % 100) % 10;
    chip->pc += 2;
}

void reg_dump(chip8 *chip) {
    for (int i = 0; i < (chip->opcode & 0x0F00) >> 8; i++) {
        chip->memory[chip->I + i] = chip->V[i];
    }

    chip->I += ((chip->opcode & 0x0F00) >> 8) + 1;
    chip->pc += 2;
}

void reg_load(chip8 *chip) {
    for (int i = 0; i < (chip->opcode & 0x0F00) >> 8; i++) {
        chip->V[i] = chip->memory[chip->I + i];
    }

    chip->I += ((chip->opcode & 0x0F00) >> 8) + 1;
    chip->pc += 2;
}

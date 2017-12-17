#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct chip8 {
    uint16_t opcode;
    uint8_t memory[4096];
    uint8_t V[16]; // registers (V0, V1, .., V15, carry flag)
    uint16_t I; // index register
    uint16_t pc; // program counter

    // 0x000 - 0x1FF - Chip 8 interpreter (contains font set in emu)
    // 0x050 - 0x0A0 - Used for built in 4x5 pixel font set (0-F)
    // 0x200 - 0xFFF - Program ROM And work RAM

    // drawing done in xor mode. if pixel is turned off as a result of drawing,
    // vf register is set. this is used for collision detection.

    uint8_t gfx[64 * 32]; // w x h

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint16_t stack[16];
    uint8_t sp;

    uint8_t keys[16];

    bool drawFlag; // collision - needed?
    // yes, needed - this is a dirty state flag
} chip8;

typedef void (*operation)(chip8 *);

void initialize(chip8 *);

void loadgame(chip8 *, char *);

void emulatecycle(chip8 *);

operation decode(uint16_t);

// instruction set
void disp_clear(chip8 *);
void return_fn(chip8 *);
void call_rca1802(chip8 *);
void goto_fn(chip8 *);
void call_subr(chip8 *);
void if_reg_eq_const(chip8 *);
void if_reg_neq_const(chip8 *);
void if_reg_eq_reg(chip8 *);
void set_reg_const(chip8 *);
void add_const_reg(chip8 *);
void assign_reg_reg(chip8 *);
void assign_or_reg_reg(chip8 *);
void assign_and_reg_reg(chip8 *);
void assign_xor_reg_reg(chip8 *);
void add_reg_reg(chip8 *);
void sub_reg_reg(chip8 *);
void shr_reg_reg(chip8 *);
void sub_reg_reg_ex(chip8 *);
void shl_reg_reg(chip8 *);
void if_reg_neq_reg(chip8 *);
void set_i_const(chip8 *);
void set_pc_v0_const(chip8 *);
void set_reg_rand_and_const(chip8 *);
void draw_reg_reg_height_const(chip8 *);
void if_key_eq_reg(chip8 *);
void if_key_neq_reg(chip8 *);
void set_reg_delay_timer(chip8 *);
void wait_set_key_reg(chip8 *);
void set_delay_timer_reg(chip8 *);
void set_sound_timer_reg(chip8 *);
void i_add_assign_reg(chip8 *);
void set_i_sprite_reg(chip8 *);
void set_i_bcd(chip8 *);
void reg_dump(chip8 *);
void reg_load(chip8 *);

#endif

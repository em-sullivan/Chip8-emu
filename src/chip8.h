#ifndef CHIP8_H
#define CHIP8_H

#include "config.h"
#include "ch8_stack.h"
#include "ch8_mem.h"
#include "ch8_keyboard.h"
#include "registers.h"

typedef struct chip8_t {
    chip8_mem_t memory;
    chip8_stack_t stack;
    chip8_reg_t registers;
    chip8_keyboard_t keyboard;
} chip8_t;

void chip8_init(chip8_t *chip8);

#endif // CHIP8_H
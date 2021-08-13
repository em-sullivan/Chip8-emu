#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"
#include "chip8_stack.h"
#include "chip8_mem.h"
#include "chip8_keyboard.h"
#include "chip8_registers.h"
#include "chip8_screen.h"

typedef struct chip8_t {
    chip8_mem_t memory;
    chip8_stack_t stack;
    chip8_reg_t registers;
    chip8_keyboard_t keyboard;
    chip8_screen_t screen;
} chip8_t;

void chip8_init(chip8_t *chip8);
void chip8_load(chip8_t *chip8, const uint8_t *buf, size_t size);
void chip8_exec(chip8_t chip8, uint16_t opcode);
#endif // CHIP8_H
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
#include "chip8_rom.h"

typedef enum chip8_opcode_t {
    CLS  = 0x00E0,
    RET  = 0x00EE,
    JP1  = 0x1000,
    CALL = 0x2000,
    SE1  = 0x3000,
    SNE1 = 0x4000,
    SE2  = 0x5000,
    LD1  = 0x6000,
    ADD1 = 0x7000,
    LD2  = 0x8000,
    OR   = 0x8001,
    AND  = 0x8002,
    XOR  = 0x8003,
    ADD2 = 0x8004,
    SUB  = 0x8005,
    SHR  = 0x8006,
    SUBN = 0x8007,
    SHL  = 0x800E,
    SNE2 = 0x9000,
    LD3  = 0xA000,
    JP2  = 0xB000,
    RND  = 0xC000,
    DRW  = 0xD000,
    SKP  = 0xE09E,
    SKNP = 0xE0A1,
    LD4  = 0xF007,
    LD5  = 0xF00A,
    LD6  = 0xF015,
    LD7  = 0xF018,
    ADD3 = 0xF01E,
    LD8  = 0xF029,
    LD9  = 0xF033,
    LD10 = 0xF055,
    LD11 = 0xF065
} chip8_opcode_t;

typedef struct chip8_t {
    chip8_mem_t memory;
    chip8_stack_t stack;
    chip8_reg_t registers;
    chip8_keyboard_t keyboard;
    chip8_screen_t screen;
} chip8_t;

void chip8_init(chip8_t *chip8);
void chip8_load(chip8_t *chip8, const uint8_t *buf, size_t size);
void chip8_exec(chip8_t *chip8, uint16_t opcode);
#endif // CHIP8_H
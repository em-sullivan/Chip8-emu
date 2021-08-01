#ifndef CHIP8_H
#define CHIP8_H

#include "config.h"
#include "ch8_mem.h"
#include "registers.h"

typedef struct chip8_t {
    chip8_mem_t memory;
    chip8_reg_t registers;
} chip8_t;

#endif // CHIP8_H
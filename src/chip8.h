#ifndef CHIP8_H
#define CHIP8_H

#include "config.h"
#include "chip8_memory.h"

typedef struct chip8_t {
    struct chip8_mem_t memory;
} chip8_t;

#endif // CHIP8_H
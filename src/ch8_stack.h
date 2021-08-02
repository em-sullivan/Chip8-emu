#ifndef CH8_STACK_H
#define CH8_STACK_H

#include <stdint.h>
#include "config.h"

typedef struct chip8_t chip8_t;

typedef struct chip8_stack_t {
    uint16_t stack[CHIP8_STACK_DEPTH];
} chip8_stack_t;

void chip8_stack_push(chip8_t *chip8, uint16_t value);
uint16_t chip8_stack_pop(chip8_t *chip8);

#endif // CH8_STACK_H
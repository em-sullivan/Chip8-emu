#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset
#include "chip8_stack.h"
#include "chip8.h"


void chip8_stack_push(chip8_t *chip8, uint16_t value)
{
    if (chip8->registers.SP < CHIP8_STACK_DEPTH) {
        chip8->stack.stack[chip8->registers.SP] = value;
        chip8->registers.SP++;
    } else {
        fprintf(stderr, "Error: Chip8 stack overflow!\n");
        exit(EXIT_FAILURE);
    }
}

uint16_t chip8_stack_pop(chip8_t *chip8)
{
    chip8->registers.SP--;
    if (chip8->registers.SP < CHIP8_STACK_DEPTH) {
        return chip8->stack.stack[chip8->registers.SP];
    } else {
        fprintf(stderr, "Error: Chip8 stack underflow!\n");
        exit(EXIT_FAILURE);
        //chip8->registers.SP++;
        return 0;
    }
}

/*
 * Chip8 Memory Source file
 */

#include <stdio.h>
#include <stdlib.h>
#include "chip8_mem.h"

void chip8_memory_set(chip8_mem_t *memory, uint32_t index, uint8_t value)
{
    if (index >= 0 && index < CHIP8_MEMORY_SIZE) {
        memory->memory[index] = value;
    } else {
        fprintf(stderr, "Error - chip8 memory set out of bounds!\n");
        exit(EXIT_FAILURE);
    }
}

uint8_t chip8_memory_get(chip8_mem_t *memory, uint32_t index)
{
    if (index >= 0 && index < CHIP8_MEMORY_SIZE) {
        return memory->memory[index];
    } else {
        fprintf(stderr, "Error - Chip8 memory read out of bounds!\n");
        exit(EXIT_FAILURE);
        return 0;
    }
}


uint16_t chip8_memory_get_short(chip8_mem_t *memory, uint32_t index)
{
    uint8_t byte1 = chip8_memory_get(memory, index);
    uint8_t byte2 = chip8_memory_get(memory, index + 1);
    return (byte1 << 8) | byte2;
}
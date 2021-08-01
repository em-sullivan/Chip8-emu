#ifndef CH8_MEM_H
#define CH8_MEM_H

#include <stdint.h>
#include "config.h"

typedef struct chip8_mem_t {
    uint8_t memory[CHIP8_MEMORY_SIZE];

} chip8_mem_t;

void chip8_memory_set(chip8_mem_t *memory, uint32_t index, uint8_t value);
uint8_t chip8_memory_get(chip8_mem_t *memory, uint32_t index);

#endif // CH8_MEM_H
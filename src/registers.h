#ifndef CHIP8_REGISTERS_H
#define CHIP8_REGISTERS_H

#include <stdint.h>
#include "config.h"

typedef struct chip8_reg_t {
    uint8_t V[CHIP8_TOTAL_REGISTERS];   // General purpose registers 0-15 (or 0-F)
    uint16_t I;                         // Memory address register (Only 12 least-sig bits used)
    uint8_t DT;                         // Delay Timer
    uint8_t ST;                         // Sound Timer

    // Pseudo registers - Should not be aviable to Chip8 programs
    uint16_t PC;                        // Program counter
    uint8_t SP;                         // Stack pointer

} chip8_reg_t;


#endif  // CHIP8_REGISTERS_H
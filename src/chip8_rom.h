#ifndef CHIP8_ROM_H
#define CHIP8_ROM_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "chip8.h"

typedef struct {
    char name[15];
    uint8_t *buffer;
    uint64_t size;
} chip8_rom_t;

bool chip8_rom_open(chip8_rom_t *chip8_rom, const char *rom_name);
void chip8_rom_parse_filename(char *filename);
bool chip8_rom_load_into_memory(chip8_rom_t *chip8_rom, chip8_t *chip8);
void chip8_rom_free(chip8_rom_t *chip8_rom);

#endif // CHIP8_ROM_H
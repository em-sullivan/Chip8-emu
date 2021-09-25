#include "chip8_rom.h"

bool chip8_rom_open(chip8_rom_t *chip8_rom, const char *rom_name)
{
    FILE *rom_file;

    // Attempt to open the rom file
    rom_file = fopen(rom_name, "rb");
    if (!rom_file) {
        return false;
    }

    // Find rom size
    fseek(rom_file, 0, SEEK_END);
    chip8_rom->size = ftell(rom_file);
    fseek(rom_file, 0, SEEK_SET); // JUMP to beginning

    // Load contents of rom into buffer
    chip8_rom->buffer = (uint8_t *) malloc(chip8_rom->size);
    int res = fread(chip8_rom->buffer, chip8_rom->size, 1, rom_file);
    if (res != 1) {
        perror("Error with reading!");
        fclose(rom_file);
        return false;
    }

    fclose(rom_file);
    return true;
}

bool chip8_rom_load_into_memory(chip8_rom_t *chip8_rom, chip8_t *chip8)
{
    // TODO: Error check
    chip8_load(chip8, chip8_rom->buffer, chip8_rom->size);
    return true;
}

void chip8_rom_free(chip8_rom_t *chip8_rom)
{
    if (chip8_rom->buffer != NULL)
        free(chip8_rom->buffer);
}

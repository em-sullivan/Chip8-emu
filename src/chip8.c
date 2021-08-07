#include "chip8.h"
#include <stdlib.h>
#include <string.h>

void chip8_init(chip8_t *chip8)
{
    // Set chip8 struct to NULL
    memset(chip8, 0, sizeof(chip8_t));
}
#ifndef CHIP8_SCREEN_H
#define CHIP8_SCREEN_H

#include <stdbool.h>
#include "config.h"

/*
 * The display for the chip 8 is a 64 by 32
 * pixel monochrome display
 */
typedef struct chip8_screen_t {

    bool pixels[CHIP8_HEIGHT][CHIP8_WIDTH];

} chip8_screen_t;

void chip8_screen_set(chip8_screen_t *screen, int x, int y);
bool chip8_screen_read(chip8_screen_t *screen, int x, int y);

#endif // CHIP8_SCREEN_H
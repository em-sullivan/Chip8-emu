#ifndef CH8_KEYBOARD_H
#define CH8_KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

/*
 * CHIP8 Keypad layout:
 *
 * | 1 | 2 | 3 | C |
 * | 4 | 5 | 6 | D |
 * | 7 | 8 | 9 | E |
 * | A | 0 | B | F |
 */

typedef struct chip8_keyboard_t {
    bool keyboard[CHIP8_TOTAL_KEYS];
} chip8_keyboard_t;

uint8_t chip8_keyboard_map(const char *map, int key);
void chip8_keyboard_down(chip8_keyboard_t *keyboard, uint8_t key);
void chip8_keyboard_up(chip8_keyboard_t *keyboard, uint8_t key);
bool chip8_keyboard_is_down(chip8_keyboard_t *keyboard, uint8_t key);

#endif // CH8_KEYBOARD_H
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "chip8_keyboard.h"


void chip8_keyboard_set_map(chip8_keyboard_t *keyboard, const char *map)
{
    keyboard->keyboard_map = map;
}

uint8_t chip8_keyboard_map(chip8_keyboard_t *keyboard, int key)
{
    if (key < 0 && key > 0xf) {
        fprintf(stderr, "Error! Key %d is out of boards for key pad!\n", key);
    }

    for (int i = 0; i < CHIP8_TOTAL_KEYS; i++) {
        if (keyboard->keyboard_map[i] == key) {
            return i;
        }
    }

    // When no key is found
    return -1;
}

void chip8_keyboard_down(chip8_keyboard_t *keyboard, uint8_t key)
{
    if (key < 0 && key > 0xf) {
        fprintf(stderr, "Error! Key %d is out of boards for key pad!\n", key);
        return;
    }
    keyboard->keyboard[key] = true;
}

void chip8_keyboard_up(chip8_keyboard_t *keyboard, uint8_t key)
{
    if (key < 0 && key > 0xf) {
        fprintf(stderr, "Error! Key %d is out of boards for key pad!\n", key);
        return;
    }
    keyboard->keyboard[key] = false;
}

bool chip8_keyboard_is_down(chip8_keyboard_t *keyboard, uint8_t key)
{
    return keyboard->keyboard[key];
}


#include "chip8_screen.h"
#include <assert.h>
#include <stdint.h>

static void chip8_screen_check_bounds(int x, int y)
{
    assert(x >= 0 && x < CHIP8_WIDTH && y >= 0 && y < CHIP8_HEIGHT);
}

void chip8_screen_set(chip8_screen_t *screen, int x, int y)
{
    chip8_screen_check_bounds(x, y);
    screen->pixels[y][x] = true;
}

bool chip8_screen_read(chip8_screen_t *screen, int x, int y)
{
    chip8_screen_check_bounds(x, y);
    return screen->pixels[y][x];
}

bool chip8_screen_draw_sprite(chip8_screen_t *screen, int x, int y, const uint8_t *sprite, int num)
{
    int32_t xs, ys;
    bool collision = false;

    for (ys = 0; ys < num; ys++) {
        uint8_t c = sprite[ys];

        for (xs = 0; xs < 8; xs++) {

            // If the bit is not set at the sprite,
            // then continue
            if ((c & (0x80 >> xs)) == 0) {
                continue;
            }

            // Check for pixel collision, since pixel is XOR
            if (screen->pixels[(ys + y) % CHIP8_HEIGHT][(xs + x) % CHIP8_WIDTH]) {
                collision = true;
            }

            // XOR pixel to screen - wrap around if sprite is out of bounds
            screen->pixels[(ys + y) % CHIP8_HEIGHT][(xs + x) % CHIP8_WIDTH] ^= true;
        }
    }

    return collision;
}

void chip8_screen_clear(chip8_screen_t *screen)
{
    // Clear the screen by setting all the pixels to 0
    memset(screen->pixels, 0, sizeof(screen->pixels));
}
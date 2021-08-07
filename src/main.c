/*
 * Main entry program
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "ch8_keyboard.h"

const char  keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_0, SDLK_1,SDLK_2, SDLK_3,
    SDLK_4, SDLK_5, SDLK_6, SDLK_7,
    SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

int main(int argc, char **argv)
{
    chip8_t cpu;

// Some test shit
#if 0
    // memtest
    chip8_memory_set(&cpu.memory, 50, '2');
    printf("Memcheck: %c\n", chip8_memory_get(&cpu.memory, 50));

    // stacktest
    cpu.registers.SP = 0;
    chip8_stack_push(&cpu, 0xFF);
    chip8_stack_push(&cpu, 0xAA);

    for (int i = 0; i < 2; i++)
        printf("Pop %x\n", chip8_stack_pop(&cpu));
#endif
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE, // Window title
        SDL_WINDOWPOS_UNDEFINED, // Position
        SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WINDOW_WIDTH,
        CHIP8_WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    SDL_Renderer *rend = SDL_CreateRenderer(window, -1, SDL_TEXTUREACCESS_TARGET);

    char key; // Actual key being pressed
    uint8_t vkey; // Virtual key on Chip8

    while (1) {
        SDL_Event event;

        // Poll for events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_QUIT:
                    // Quit out
                    goto out;
                    break;

                case SDL_KEYDOWN:
                    key = event.key.keysym.sym;
                    vkey = chip8_keyboard_map(keyboard_map, key);
                    if (vkey != -1) {
                        chip8_keyboard_down(&cpu.keyboard, vkey);
                    }

                    break;

                case SDL_KEYUP:
                    key = event.key.keysym.sym;
                    vkey = chip8_keyboard_map(keyboard_map, key);
                    if (vkey != -1) {
                        chip8_keyboard_up(&cpu.keyboard, vkey);
                    }

                    break;
            }

        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 0); // Black color
        SDL_RenderClear(rend);

        // Draw Rectangle
        SDL_Rect r;
        SDL_SetRenderDrawColor(rend, 255, 0, 255, 0); // Purple
        r.x = 0;
        r.y = 0;
        r.w = 40;
        r.h = 40;
        SDL_RenderFillRect(rend, &r);
        SDL_RenderPresent(rend);
    }

out:
    printf("Bye!/n");
    SDL_DestroyWindow(window);
    return 0;
}
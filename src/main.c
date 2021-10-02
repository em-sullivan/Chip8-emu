/*
 * Main entry program
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include "chip8.h"
#include "chip8_keyboard.h"

/*
 * Keymap - Play around with this
 */
const char  keyboard_map[CHIP8_TOTAL_KEYS] = {
    SDLK_0, SDLK_1,SDLK_2, SDLK_3,
    SDLK_4, SDLK_5, SDLK_6, SDLK_7,
    SDLK_8, SDLK_9, SDLK_a, SDLK_b,
    SDLK_c, SDLK_d, SDLK_e, SDLK_f
};

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: Chip8 <rom>\n");
        return -1;
    }

    const char* filename = argv[1];
    printf("Loading %s...\n", filename);

    // Load Chip8 ROM into memory
    chip8_rom_t rom;
    if (chip8_rom_open(&rom, filename) == false) {
        perror("Error! Could not load rom file!");
        return -1;
    }

    // Init Chip8
    chip8_t cpu;
    chip8_init(&cpu);

    // Load program into chip8 memory
    chip8_rom_load_into_memory(&rom, &cpu);

    // Entire game is im memory now, so lets free up the buffer
    // and close the file
    chip8_rom_free(&rom);

    // Set keyboard map
    chip8_keyboard_set_map(&cpu.keyboard, keyboard_map);

    // Init SDL and app window
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
    SDL_Event event;

    while (1) {

        // Poll for events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {

                case SDL_QUIT:
                    // Quit out
                    cpu.running = false;
                    break;

                case SDL_KEYDOWN:
                    key = event.key.keysym.sym;
                    vkey = chip8_keyboard_map(&cpu.keyboard, key);
                    if (vkey != -1) {
                        chip8_keyboard_down(&cpu.keyboard, vkey);
                    }

                    break;

                case SDL_KEYUP:
                    key = event.key.keysym.sym;
                    vkey = chip8_keyboard_map(&cpu.keyboard, key);
                    if (vkey != -1) {
                        chip8_keyboard_up(&cpu.keyboard, vkey);
                    }

                    break;
            }

        }

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 0); // Black color
        SDL_RenderClear(rend);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 0); // White color

        for (int x = 0; x < CHIP8_WIDTH; x++) {
            for (int y = 0; y < CHIP8_HEIGHT; y++) {

                if (chip8_screen_read(&cpu.screen, x, y)) {

                    // Draw "pixel"
                    // Since the original chip8 had a 64x32 display, drawing
                    // actual pixels would make the display really small, so
                    // each pixel is scaled
                    SDL_Rect r;
                    r.x = x * CHIP8_WINDOW_MULTIPLIER;
                    r.y = y * CHIP8_WINDOW_MULTIPLIER;
                    r.w = CHIP8_WINDOW_MULTIPLIER;
                    r.h = CHIP8_WINDOW_MULTIPLIER;
                    SDL_RenderFillRect(rend, &r);
                }

            }
        }

        SDL_RenderPresent(rend);
        chip8_timer_tick(&cpu);

        if (cpu.running == false)
            break;

        // Execute OPCode
        uint16_t instruction = chip8_memory_get_short(&cpu.memory, cpu.registers.PC);
        cpu.registers.PC += 2; // Increment program counter to next
        chip8_execute(&cpu, instruction);
    }

    // Exiting the emulator
    printf("Bye!/n");
    SDL_DestroyWindow(window);
    return 0;
}

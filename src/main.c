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
    FILE *rom = fopen(filename, "rb");
    if (!rom) {
        perror("Error! Could not open file!");
        return -2;
    }

    // Find how long the file is
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    fseek(rom, 0, SEEK_SET); // JUMP to beginning

    uint8_t *buf = (uint8_t *) malloc(rom_size);
    int res = fread(buf, rom_size, 1, rom);
    if (res != 1) {
        perror("Error with reading!");
        fclose(rom);
        return -3;
    }

    chip8_t cpu;

    chip8_init(&cpu);

    // Load program into chip8 memory
    chip8_load(&cpu, buf, rom_size);

    // Entire game is im memory now, so lets free up the buffer
    // and close the file
    free(buf);
    fclose(rom);

    // Set keyboard map
    chip8_keyboard_set_map(&cpu.keyboard, keyboard_map);

    // Sreen draw test
    //chip8_screen_draw_sprite(&cpu.screen, 10, 30, &cpu.memory.memory[5], 5);

    // CLS Test
    //chip8_exec(&cpu, 0x00E0);

    // JP TEST
    //chip8_exec(&cpu, 0x1FF2);
    //printf("%x\n", cpu.registers.PC);

    // SE TEST
    //cpu.registers.PC = 0x00;
    //cpu.registers.V[0] = 0x22;
    //chip8_exec(&cpu, 0x3022);
    //printf("%x\n", cpu.registers.PC);


    // LD TEST
    // cpu.registers.V[0] = 1;
    // cpu.registers.V[2] = 2;
    // chip8_exec(&cpu, 0x8020);
    // printf("%x\n", cpu.registers.V[0]);

    // ADD TEST
    //cpu.registers.V[0] = 255;
    //cpu.registers.V[1] = 1;
    //chip8_exec(&cpu, 0x8014);
    //printf("%x\n", cpu.registers.V[0]);
    //printf("%x\n", cpu.registers.V[0xF]);

    // Test DRWi
    // cpu.registers.I = 0;
    // cpu.registers.V[0] = 10;
    // cpu.registers.V[1] = 10;
    // chip8_exec(&cpu, 0xD015);

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow(
        EMULATOR_WINDOW_TITLE, // Window title
        SDL_WINDOWPOS_UNDEFINED, // Position
        SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WINDOW_WIDTH,
        CHIP8_WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    // TEST
    //cpu.registers.V[0] = 0x00;
    //chip8_exec(&cpu, 0xF00A);

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
                    goto out;
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

        // If delay timer is above zero,
        // sleep for 100 ms
        if (cpu.registers.DT > 0) {
            usleep(10000);
            cpu.registers.DT--;
        }

        // TODO: Create beep function for linux,
        // I can't use the regual beep because my laptop
        // does not have a PC speaker
        if (cpu.registers.ST > 0) {
            // Implement Beep at some point
            // beep(freq, 100 * cpu.registers.ST)
            printf("BEEP\n");
            cpu.registers.ST = 0;
        }

        // Execute OPCode
        uint16_t opcode = chip8_memory_get_short(&cpu.memory, cpu.registers.PC);
        cpu.registers.PC += 2; // Increment program counter to next
        chip8_exec(&cpu, opcode);
    }

out:
    printf("Bye!/n");
    SDL_DestroyWindow(window);
    return 0;
}
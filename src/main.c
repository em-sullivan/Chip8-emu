/*
 * Main entry program
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"

int main(int argc, char **argv)
{
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

    while (1) {
        SDL_Event event;

        // Poll for events
        while(SDL_PollEvent(&event)) {

            // Quit
            if (event.type == SDL_QUIT) {
                goto out;
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
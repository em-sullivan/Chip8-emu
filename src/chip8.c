#include "chip8.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <SDL2/SDL.h>

/*
 * Sprites represeting the hexadecimal digits 0 - F
 * 5 Bytes long, 8x5 pixels
 */
static const uint8_t chip8_character_set[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(chip8_t *chip8)
{
    // Set chip8 struct to NULL
    memset(chip8, 0, sizeof(chip8_t));

    // Load in character set into memory
    memcpy(chip8->memory.memory, chip8_character_set, sizeof(chip8_character_set));
}


void chip8_load(chip8_t *chip8, const uint8_t *buf, size_t size)
{
    // Load program into chip 8 memory (if it can fit)
    assert(size + CHIP8_PROGRAM_LOAD_ADDR < CHIP8_MEMORY_SIZE);
    memcpy(&chip8->memory.memory[CHIP8_PROGRAM_LOAD_ADDR], buf, size);

    // Set Program counter to start of program
    chip8->registers.PC = CHIP8_PROGRAM_LOAD_ADDR;
}

static uint8_t wait_for_key_press(chip8_t *chip8)
{
    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        if (event.type != SDL_KEYDOWN) {
            continue;
        }

        char c = event.key.keysym.sym;
        uint8_t chip8_key = chip8_keyboard_map(&chip8->keyboard, c);
        if (chip8_key != -1) {
            return chip8_key;
        }
    }

    return -1;
}

void chip8_execute(chip8_t *chip8, uint16_t opcode)
{
    uint16_t nnn = opcode & 0x0FFF;     // 12-bit constant
    uint8_t x = (opcode >> 8) & 0x0F;   // Register X
    uint8_t y = (opcode >> 4) & 0x0F;   // Register Y
    uint8_t kk = opcode & 0x00FF;       // Byte constant
    uint8_t n = opcode & 0x000F;        // Last nibble
    uint16_t result, i;

    chip8_opcode_t op;
    uint16_t mask = 0xF000 & opcode;
    if (mask == 0x8000) {
        op = opcode & 0xF00F;
    } else if((mask == 0xF000) || (mask == 0xE000)) {
        op = opcode & 0xF0FF;
    } else if (mask != 0) {
        op = mask;
    } else {
        op = opcode & 0x00FF;
    }

    switch (op) {
        case CLS:
            // Clear the display
            chip8_screen_clear(&(chip8->screen));
            break;

        case RET:
            // Return from subroutine:
            // The interperter sets the PC to the address at the top of the stack,
            // then subtracts 1 from the stack pointer
            chip8->registers.PC = chip8_stack_pop(chip8);
            break;

        case JP1:
            // Jump to location nnn:
            chip8->registers.PC = nnn;
            break;

            break;

        case CALL:
            // Call subroutine at location nnn:
            // Saves the current PC to the stack, then sets PC to nnn
            chip8_stack_push(chip8, chip8->registers.PC);
            chip8->registers.PC = nnn;
            break;

        case SE1:
            // Skip next instruction if Vx = kk
            if (chip8->registers.V[x] == kk) {
                chip8->registers.PC += 2;
            }
            break;

        case SNE1:
            // Skip next instruction if Vx != kk
            if (chip8->registers.V[x] != kk) {
                chip8->registers.PC += 2;
            }
            break;

        case SE2:
            // Skip the next instruction if Vx == Vy
            if (chip8->registers.V[x] == chip8->registers.V[y]) {
                chip8->registers.PC += 2;
            }
            break;

        case LD1:
            // Set Vx to kk
            chip8->registers.V[x] = kk;
            break;

        case ADD1:
            // Add kk to Vx
            chip8->registers.V[x] += kk;
            break;

        case LD2:
            // Set Vx = Vy
            chip8->registers.V[x] = chip8->registers.V[y];
            break;

        case OR:
            // Performs bitwise OR on Vx and Vy
            chip8->registers.V[x] |= chip8->registers.V[y];
            break;

        case AND:
            // Performs bitwise and on Vx and Vy
            chip8->registers.V[x] &= chip8->registers.V[y];
            break;

        case XOR:
            // Performs bitwise exclusive OR on Vx and Vy
            chip8->registers.V[x] ^= chip8->registers.V[y];
            break;

        case ADD2:
            // Add Vx and Vy and store in Vx
            // IF greater then 255, then set carry Vf to 1
            // Lowest 8 bits are stored in Vx
            result = chip8->registers.V[x] + chip8->registers.V[y];
            chip8->registers.V[0xF] = result > 255 ? 1 : 0;
            chip8->registers.V[x] = result;
            break;

        case SUB:
            // Vx = Vx - Yy
            chip8->registers.V[0xF] = chip8->registers.V[x] > chip8->registers.V[y] ? 1 : 0;
            chip8->registers.V[x] -= chip8->registers.V[y];
            break;

        case SHR:
            // Set Vx = Vx SHR 1
            // If least sig bit is 1, then set VF to 1
            // Divide Vx by 2 (in this case, I shift right 1 because that is the same)
            chip8->registers.V[0xF] = chip8->registers.V[x] & 0x01;
            chip8->registers.V[x] >>= 1;
            break;

        case SUBN:
            // If Vy > Vx, set Vf
            // Vx = Vy - Vx
            chip8->registers.V[0xF] = chip8->registers.V[y] > chip8->registers.V[x] ? 1 : 0;
            chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
            break;

        case SHL:
            // If the most sigg bit of Vx is 1, then set VF to 1
            // Multiply Vx by two
            chip8->registers.V[0xF] = chip8->registers.V[x] & 0x80 ? 1 : 0;
            chip8->registers.V[x] <<= 1;
            break;

        case SNE2:
            // Skips next instruction if Vx != Vy
            if (chip8->registers.V[x] != chip8->registers.V[y]) {
                chip8->registers.PC += 2;
            }
            break;

        case LD3:
            // Set register I to nnn
            chip8->registers.I = nnn;
            break;

        case JP2:
            // Jump to location V0 + nnn
            chip8->registers.PC = chip8->registers.V[0] + nnn;
            break;

        case RND:
            // Set Vx = random byte AND kk
            srand(clock());
            chip8->registers.V[x] = (rand() % 255) & kk;
            break;

        case DRW:
            // Display n-byte sprite starting at memory location I at (Vx, Vy), set Vf = Collision
            // I points to sprite you want to draw
            const uint8_t *sprite = (const uint8_t *)&chip8->memory.memory[chip8->registers.I];
            chip8->registers.V[0xF] = chip8_screen_draw_sprite(&chip8->screen,
                chip8->registers.V[x], chip8->registers.V[y], sprite, n);
            break;

        case SKP:
            // Skip the next instruction if the key with value Vx is pressed
            if (chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x])) {
                chip8->registers.PC += 2;
            }
            break;

        case SKNP:
            // Skip the next instruction if key with value Vx is not pressed
            if (!chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x])) {
                chip8->registers.PC += 2;
            }
            break;

        case LD4:
            // Set Vx to delay timer
            chip8->registers.V[x] = chip8->registers.DT;
            break;

        case LD5:
            // Wait for a key press, store the value of the key in Vx
            uint8_t pressed_key = wait_for_key_press(chip8);
            chip8->registers.V[x] = pressed_key;
            break;

        case LD6:
            // Set delay timer to Vx
            chip8->registers.DT = chip8->registers.V[x];
            break;

        case LD7:
            // Set sound timer to Vx
            chip8->registers.ST = chip8->registers.V[x];
            break;

        case ADD3:
            // Set I as I + Vx
            chip8->registers.I += chip8->registers.V[x];
            break;

        case LD8:
            // Set I to location of sprite Vx
            chip8->registers.I = chip8->registers.V[x] * CHIP8_DEFAULT_SPRITE_HEIGHT;
            break;

        case LD9:
            // Store BCD of VX in memory location I, I+1, I+2
            uint8_t hund = chip8->registers.V[x] / 100;
            uint8_t tens = chip8->registers.V[x] / 10 % 10;
            uint8_t ones = chip8->registers.V[x] % 10;
            chip8_memory_set(&chip8->memory, chip8->registers.I, hund);
            chip8_memory_set(&chip8->memory, chip8->registers.I + 1, tens);
            chip8_memory_set(&chip8->memory, chip8->registers.I + 2, ones);
            break;

        case LD10:
            // Store registers V0 through Vx in memory starting at I
            for(i = 0; i <= x; i++) {
                chip8_memory_set(&chip8->memory, chip8->registers.I + i,  chip8->registers.V[i]);
            }
            break;

        case LD11:
            // Reads in memory to V0 through Vx starting at I
            for (i = 0; i <= x; i++) {
                chip8->registers.V[i] = chip8_memory_get(&chip8->memory, chip8->registers.I + i);
            }
            break;

        default:
    }
}

void chip8_timer_tick(chip8_t *chip8)
{
    // If delay timer is above zero,
    if (chip8->registers.DT > 0) {
        SDL_Delay(10);
        chip8->registers.DT--;
    }

    // TODO: Create beep function for linux,
    // I can't use the regual beep because my laptop
    // does not have a PC speaker
    if (chip8->registers.ST > 0) {
        // Implement Beep at some point
        // beep(freq, 100 * cpu.registers.ST)
        printf("BEEP\n");
        chip8->registers.ST = 0;
    }
}

// Old execture code which is kind of gross, leaving it here for now
// in case I need it
#if 0
static void chip8_exec_extended_8(chip8_t *chip8, uint16_t opcode)
{
    uint8_t x = (opcode >> 8) & 0x0F;
    uint8_t y = (opcode >> 4) & 0x0F;
    uint8_t nib = opcode & 0x0F;
    uint16_t result = 0;

    switch (nib) {
        case 0x0: // LD Vx, Vy
            // Set Vx = Vy
            chip8->registers.V[x] = chip8->registers.V[y];
            break;

        case 0x01: // OR Vx, Vy
            // Performs bitwise OR on Vx and Vy
            chip8->registers.V[x] |= chip8->registers.V[y];
            break;

        case 0x02: // AND Vx, Vy
            // Performs bitwise and on Vx and Vy
            chip8->registers.V[x] &= chip8->registers.V[y];
            break;

        case 0x03: // XOR Vx, Vy
            // Performs bitwise exclusive OR on Vx and Vy
            chip8->registers.V[x] ^= chip8->registers.V[y];
            break;

        case 0x04: // ADD Vx, Vy, VF = carry
            // Add Vx and Vy and store in Vx
            // IF greater then 255, then set carry Vf to 1
            // Lowest 8 bits are stored in Vx
            result = chip8->registers.V[x] + chip8->registers.V[y];
            chip8->registers.V[0xF] = result > 255 ? 1 : 0;
            chip8->registers.V[x] = result;
            break;

        case 0x05: // SUB Vx, Vy, VF = Not Borrow
            // Vx = Vx - Yy
            chip8->registers.V[0xF] = chip8->registers.V[x] > chip8->registers.V[y] ? 1 : 0;
            chip8->registers.V[x] -= chip8->registers.V[y];
            break;

        case 0x06: // SHR Vx {, Vy}
            // Set Vx = Vx SHR 1
            // If least sig bit is 1, then set VF to 1
            // Divide Vx by 2 (in this case, I shift right 1 because that is the same)
            chip8->registers.V[0xF] = chip8->registers.V[x] & 0x01;
            chip8->registers.V[x] >>= 1;
            break;

        case 0x07: // SUBN Vx, Vy
            // If Vy > Vx, set Vf
            // Vx = Vy - Vx
            chip8->registers.V[0xF] = chip8->registers.V[y] > chip8->registers.V[x] ? 1 : 0;
            chip8->registers.V[x] = chip8->registers.V[y] - chip8->registers.V[x];
            break;

        case 0x0E: // SHL Vx {, Vy}
            // If the most sigg bit of Vx is 1, then set VF to 1
            // Multiply Vx by two
            chip8->registers.V[0xF] = chip8->registers.V[x] & 0x80 ? 1 : 0;
            chip8->registers.V[x] <<= 1;
            break;
    }
}


static void chip8_exec_extended_F(chip8_t *chip8, uint16_t opcode)
{
    uint8_t x = (opcode >> 8) & 0x0F;
    int i;

    switch (opcode & 0x00FF) {
        case 0x07: // LD Vx, DT
            // Set Vx to delay timer
            chip8->registers.V[x] = chip8->registers.DT;
            break;

        case 0x0A: // LD Vx, K
            // Wait for a key press, store the value of the key in Vx
            uint8_t pressed_key = wait_for_key_press(chip8);
            chip8->registers.V[x] = pressed_key;
            break;

        case 0x15: // LD DT, Vx
            // Set delay timer to Vx
            chip8->registers.DT = chip8->registers.V[x];
            break;

        case 0x18: // KD St, Vx
            // Set sound timer to Vx
            chip8->registers.ST = chip8->registers.V[x];
            break;

        case 0x1E: // Add I, Vx
            // Set I as I + Vx
            chip8->registers.I += chip8->registers.V[x];
            break;

        case 0x29: // LD F, Vx
            // Set I to location of sprite Vx
            chip8->registers.I = chip8->registers.V[x] * CHIP8_DEFAULT_SPRITE_HEIGHT;
            break;

        case 0x33: // LD B, Vx
            // Store BCD of VX in memory location I, I+1, I+2
            uint8_t hund = chip8->registers.V[x] / 100;
            uint8_t tens = chip8->registers.V[x] / 10 % 10;
            uint8_t ones = chip8->registers.V[x] % 10;
            chip8_memory_set(&chip8->memory, chip8->registers.I, hund);
            chip8_memory_set(&chip8->memory, chip8->registers.I + 1, tens);
            chip8_memory_set(&chip8->memory, chip8->registers.I + 2, ones);
            break;

        case 0x55: // LD [I], Vx
            // Store registers V0 through Vx in memory starting at I
            for(i = 0; i <= x; i++) {
                chip8_memory_set(&chip8->memory, chip8->registers.I + i,  chip8->registers.V[i]);
            }
            break;

        case 0x65: // LD Vx, [I]
            // Reads in memory to V0 through Vx starting at I
            for (i = 0; i <= x; i++) {
                chip8->registers.V[i] = chip8_memory_get(&chip8->memory, chip8->registers.I + i);
            }
            break;
    }
}

static void chip8_exec_extended(chip8_t *chip8, uint16_t opcode)
{
    uint16_t nnn = opcode & 0x0FFF;     // 12-bit constant
    uint8_t x = (opcode >> 8) & 0x0F;   // Register X
    uint8_t y = (opcode >> 4) & 0x0F;   // Register Y
    uint8_t kk = opcode & 0x00FF;       // Byte constant
    uint8_t n = opcode & 0x000F;        // Last nibble

    switch(opcode & 0xF000) {
        case 0x1000: // JP nnn
            // Jump to location nnn:
            chip8->registers.PC = nnn;
            break;
        case 0x2000: // Call nnn
            // Call subroutine at location nnn:
            // Saves the current PC to the stack, then sets PC to nnn
            chip8_stack_push(chip8, chip8->registers.PC);
            chip8->registers.PC = nnn;
            break;

        case 0x3000: // SE Vx, kk
            // Skip next instruction if Vx = kk
            if (chip8->registers.V[x] == kk) {
                chip8->registers.PC += 2;
            }
            break;

        case 0x4000: // SNE Vx, kk
            // Skip next instruction if Vx != kk
            if (chip8->registers.V[x] != kk) {
                chip8->registers.PC += 2;
            }
            break;

        case 0x5000: // SE Vx, Vy
            // Skip the next instruction if Vx == Vy
            if (chip8->registers.V[x] == chip8->registers.V[y]) {
                chip8->registers.PC += 2;
            }
            break;

        case 0x6000: // LD Vx, kk
            // Set Vx to kk
            chip8->registers.V[x] = kk;
            break;

        case 0x7000: // ADD Vx, kk
            // Add kk to Vx
            chip8->registers.V[x] += kk;
            break;

        case 0x8000: // This one has a bunch
            chip8_exec_extended_8(chip8, opcode);
            break;

        case 0x9000: // SNE Vx, Vy
            // Skips next instruction if Vx != Vy
            if (chip8->registers.V[x] != chip8->registers.V[y]) {
                chip8->registers.PC += 2;
            }
            break;

        case 0xA000: // LD I, addr
            // Set register I to nnn
            chip8->registers.I = nnn;
            break;

        case 0xB000: // JP V0, addr
            // Jump to location V0 + nnn
            chip8->registers.PC = chip8->registers.V[0] + nnn;
            break;

        case 0xC000: // RND, Vx, byte
            // Set Vx = random byte AND kk
            srand(clock());
            chip8->registers.V[x] = (rand() % 255) & kk;
            break;

        case 0xD000: // DRW Vx, Vy, nibble
            // Display n-byte sprite starting at memory location I at (Vx, Vy), set Vf = Collision
            // I points to sprite you want to draw
            const uint8_t *sprite = (const uint8_t *)&chip8->memory.memory[chip8->registers.I];
            chip8->registers.V[0xF] = chip8_screen_draw_sprite(&chip8->screen,
                chip8->registers.V[x], chip8->registers.V[y], sprite, n);
            break;

        // Keyboard operations
        case 0xE000:
            // There are two opcodes that use the bottom byte, so we just look at kk
            switch (kk) {
                case 0x9E: // SKP Vx
                    // Skip the next instruction if the key with value Vx is pressed
                    if (chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x])) {
                        chip8->registers.PC += 2;
                    }
                    break;

                case 0xA1: // SKNP Vx
                    // Skip the next instruction if key with value Vx is not pressed
                    if (!chip8_keyboard_is_down(&chip8->keyboard, chip8->registers.V[x])) {
                        chip8->registers.PC += 2;
                    }

                    break;
            }
            break;

        case 0xF000:
            chip8_exec_extended_F(chip8, opcode);
            break;
        default:

    }
}

void chip8_exec(chip8_t *chip8, uint16_t opcode)
{
    switch (opcode) {
        case 0x00E0: // CLR
            // Clear the display
            chip8_screen_clear(&(chip8->screen));
            break;

        case 0x00EE: // RET
            // Return from subroutine:
            // The interperter sets the PC to the address at the top of the stack,
            // then subtracts 1 from the stack pointer
            chip8->registers.PC = chip8_stack_pop(chip8);
            break;

        default:
            chip8_exec_extended(chip8, opcode);
    }
}
#endif

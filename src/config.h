#ifndef CONFIG_H
#define CONFIG_H

#define EMULATOR_WINDOW_TITLE       "Chip8 Emu"

#define CHIP8_MEMORY_SIZE           4096
#define CHIP8_WIDTH                 64
#define CHIP8_HEIGHT                32
#define CHIP8_WINDOW_MULTIPLIER     10 // For scaling the window
#define CHIP8_WINDOW_WIDTH          (CHIP8_WIDTH * CHIP8_WINDOW_MULTIPLIER)
#define CHIP8_WINDOW_HEIGHT         (CHIP8_HEIGHT * CHIP8_WINDOW_MULTIPLIER)

#define CHIP8_TOTAL_REGISTERS       16

#endif // CONFIG_H
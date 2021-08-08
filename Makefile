CFLAGS = -g -Wall -o2
LIBS = -lSDL2main -lSDL2
OBJS = ./build/chip8_mem.o ./build/chip8_stack.o ./build/chip8_keyboard.o ./build/chip8.o ./build/chip8_screen.o


all: $(OBJS)
	$(CC) $(CFLAGS) ./src/main.c $(OBJS) $(LIBS) -o ./bin/main

./build/%.o: ./src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f ./bin/main
	rm -f ./build/*.o
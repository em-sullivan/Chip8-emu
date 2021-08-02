CFLAGS = -g -Wall -o2
OBJS = ./build/ch8_mem.o ./build/ch8_stack.o

all: $(OBJS)
	$(CC) $(CFLAGS) ./src/main.c $(OBJS) -lSDL2main -lSDL2 -o ./bin/main

./build/%.o: ./src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f ./bin/main
	rm -f ./build/*.o
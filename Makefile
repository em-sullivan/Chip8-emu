CFLAGS = -g -Wall -o2

all:
	$(CC) $(CFLAGS) ./src/main.c -lSDL2main -lSDL2 -o ./bin/main

clean:
	rm -f ./bin/main
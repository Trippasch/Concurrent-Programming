CC = gcc
CFLAGS = -Wall -g
OBJ = mycoroutines.o

all: main

main: $(OBJ) main.c
	$(CC) $(CFLAGS) $(OBJ) main.c -o main

mycoroutines.o: mycoroutines.c mycoroutines.h
	$(CC) $(CFLAGS) -c mycoroutines.c -o mycoroutines.o

.PHONY: clean

clean:
	rm -rf $(OBJ) main output.txt

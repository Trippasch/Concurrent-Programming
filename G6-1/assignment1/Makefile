CC = gcc
CFLAGS = -Wall -g
OBJ = operations.o main.o

all: assignment1

assignment1: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o assignment1 -pthread

main.o: main.c operations.h
	$(CC) $(CFLAGS) -c main.c -o main.o

operations.o: operations.c operations.h
	$(CC) $(CFLAGS) -c operations.c -o operations.o

.PHONY: clean

clean:
	rm -rf *.o assignment1 output

dist:
	tar -cvf assignment1.tgz *.c *.h Makefile chap1_10885
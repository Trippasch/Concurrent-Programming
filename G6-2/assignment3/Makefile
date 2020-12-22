CC = gcc
CFLAGS = -Wall -g
OBJ = mysem.o
LIBS = -lpthread

all: bridge

bridge: bridge.c $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) bridge.c -o bridge $(LIBS)

mysem.o: ../assignment1/mysem.c ../assignment1/mysem.h
	$(CC) $(CFLAGS) -c ../assignment1/mysem.c -o mysem.o

.PHONY: clean
clean:
	rm -rf *.o bridge output.txt

CC = gcc
CFLAGS = -Wall -g
OBJ = mysem.o
LIBS = -lpthread

all: prime

prime: prime.c $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) prime.c -o prime $(LIBS)

mysem.o: ../assignment1/mysem.c ../assignment1/mysem.h
	$(CC) $(CFLAGS) -c ../assignment1/mysem.c -o mysem.o

.PHONY: clean

clean:
	rm -rf *.o prime output.txt

CC = gcc
CFLAGS = -Wall -g
OBJ = mysem.o
LIBS = -lpthread

all: train

train: train.c $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) train.c -o train $(LIBS)

mysem.o: ../assignment1/mysem.c ../assignment1/mysem.h
	$(CC) $(CFLAGS) -c ../assignment1/mysem.c -o mysem.o

.PHONY: clean
clean:
	rm -rf *.o train

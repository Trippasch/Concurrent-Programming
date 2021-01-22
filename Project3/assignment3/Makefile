CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: train

train: train.c
	$(CC) $(CFLAGS) train.c -o train $(LIBS)

.PHONY: clean
clean:
	rm -rf *.o train

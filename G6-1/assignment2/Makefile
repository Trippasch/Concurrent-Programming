CC = gcc
CFLAGS = -Wall -g

all: prime

prime: prime.c
	$(CC) $(CFLAGS) prime.c -o prime -pthread

.PHONY: clean

clean:
	rm -f prime output.txt

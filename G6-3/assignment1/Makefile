CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: prime

prime: prime.c
	$(CC) $(CFLAGS) prime.c -o prime $(LIBS)
.PHONY: clean

clean:
	rm -rf *.o prime output.txt

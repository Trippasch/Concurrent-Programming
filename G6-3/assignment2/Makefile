CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: bridge

bridge: bridge.c
	$(CC) $(CFLAGS) bridge.c -o bridge $(LIBS)

.PHONY: clean
clean:
	rm -rf *.o bridge output.txt

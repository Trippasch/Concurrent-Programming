CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread

all: prime_CCR bridge_CCR

prime_CCR: ccr.h prime_CCR.c
	$(CC) $(CFLAGS) prime_CCR.c -o prime_CCR $(LIBS)

bridge_CCR: ccr.h bridge_CCR.c
	$(CC) $(CFLAGS) bridge_CCR.c -o bridge_CCR $(LIBS)

.PHONY: clean

clean:
	rm -rf prime_CCR bridge_CCR *.o output.txt

CC = gcc
CFLAGS = -Wall -g

all: quicksort

quicksort: quicksort.c
	$(CC) $(CFLAGS) quicksort.c -o quicksort -pthread

.PHONY: clean

clean:
	rm -f quicksort

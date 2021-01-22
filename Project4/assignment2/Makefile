CC = gcc
CFLAGS = -Wall -g
OBJ = mylist.o mythreads.o

all: main

main: $(OBJ) main.c
	$(CC) $(CFLAGS) $(OBJ) main.c -o main

mythreads.o: mythreads.c mythreads.h mylist.h
	$(CC) $(CFLAGS) -c mythreads.c -o mythreads.o

mylist.o: mylist.c mylist.h mythreads.h
	$(CC) $(CFLAGS) -c mylist.c -o mylist.o

.PHONY: clean

clean:
	rm -rf $(OBJ) main output.txt

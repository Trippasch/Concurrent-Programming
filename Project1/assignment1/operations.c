#include "operations.h"

#include <stdio.h>
#include <stdlib.h>

volatile char *pipe_array;
volatile int read_pos, write_pos, pipe_size;

void pipe_init(int size) {

    pipe_array = (volatile char *) malloc(sizeof(char) * size);
    if (pipe_array == NULL) {
        fprintf(stderr, "Memory not allocated.\n");
        exit(1);
    }

    for (int i = 0; i < size; i++) {
        pipe_array[i] = '\0';
    }

    read_pos = 0;
    write_pos = 0;
}

void pipe_write(char c) {

    pipe_array[write_pos] = c;
    write_pos++;

    if (write_pos >= pipe_size) {
        write_pos = 0;
    }
}

void pipe_writeDone() {

    write_pos = -1;
}

int pipe_read(char *c) {

    if (write_pos == -1 && pipe_array[read_pos] == '\0') {
        read_pos = -1;
        return 0;
    }

    *c = pipe_array[read_pos];
    pipe_array[read_pos] = '\0';
    read_pos++;

    if (read_pos >= pipe_size) {
        read_pos = 0;
    }

    return 1;
}

void pipe_destroy() {

    free((void *) pipe_array);
}
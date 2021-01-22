#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ucontext.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mycoroutines.h"

char *buffer;
int buffer_size;

co_t *main_co;
co_t *read_co;
co_t *write_co;

void write_context(int);
void read_context(int);
void compare_files();

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments!\n");
        fprintf(stderr, "usage: ./assignment1 <int> <input_file>\n");
        exit(EXIT_FAILURE);
    }

    int read_fd, write_fd;

    buffer_size = atoi(argv[1]);
    buffer = (char *) malloc(buffer_size * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Error in malloc()\n");
        exit(EXIT_FAILURE);
    }

    main_co = (co_t *) malloc(sizeof(co_t));
    read_co = (co_t *) malloc(sizeof(co_t));
    write_co = (co_t *) malloc(sizeof(co_t));

    // file descriptor to the file we are reading from
    read_fd = open(argv[2], O_RDONLY, S_IRWXU);
    if (read_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // file descriptor to the file we are writing to
    write_fd = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (write_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    mycoroutines_init(main_co);

    mycoroutines_create(read_co, (void (*)(void *)) read_context, (void*)(intptr_t) write_fd);
    mycoroutines_create(write_co, (void (*)(void *)) write_context, (void*)(intptr_t) read_fd);

    current_context = &main_co->context;
    mycoroutines_switchto(write_co);
    fprintf(stderr, "main: finished!\n");

    compare_files(argv[2]);

    // destroy coroutines and any dynamic memory.
    mycoroutines_destroy(main_co);
    mycoroutines_destroy(write_co);
    mycoroutines_destroy(read_co);

    free(buffer);

    return 0;
}

void write_context(int fd) {

    char c;
    int write_pos = 0;

    lseek(fd, 0, SEEK_SET);

    while (read(fd, &c, sizeof(char))) {
        buffer[write_pos++] = c;

        if (write_pos >= buffer_size) {
            write_pos = 0;
            fprintf(stderr, "write_co: switching to read_co\n");
            current_context = &write_co->context;
            mycoroutines_switchto(read_co);
        }
    }

    fprintf(stderr, "write_co: switching to read_co\n");
    current_context = &write_co->context;
    mycoroutines_switchto(read_co);

    fprintf(stderr, "write_co: finished!\n");
}

void read_context(int fd) {
    
    char c;
    int read_pos = 0;

    while (1) {
        if (buffer[read_pos] == '\0') {
            break;
        }

        c = buffer[read_pos];
        buffer[read_pos] = '\0';
        read_pos++;

        write(fd, &c, sizeof(char));

        if (read_pos >= buffer_size) {
            read_pos = 0;
            fprintf(stderr, "read_co: switching to write_co\n");
            current_context = &read_co->context;
            mycoroutines_switchto(write_co);
        }
    }

    fprintf(stderr, "read_co: finished!\n");
}

void compare_files(char *file) {
    char cmd[14 + strlen(file)];
    sprintf(cmd, "/bin/diff output.txt %s", file);
    fprintf(stderr, "return value of diff: %d\n", system(cmd));
}

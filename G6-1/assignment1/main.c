#include "operations.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void *thread_write(void *arg);
void *thread_read(void *arg);

volatile int stop = 0;

int main(int argc, char const *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments!\n");
        fprintf(stderr, "usage: ./assignment1 <int> <input_file>\n");
        exit(1);
    }

    int read_fd, write_fd, res;
    pthread_t write_thread, read_thread;

    // Get pipe size from command line arguments
    pipe_size = atoi(argv[1]);
    pipe_init(pipe_size);

    // file descriptor to the file we are reading from
    read_fd = open(argv[2], O_RDONLY, S_IRWXU);
    if (read_fd == -1) {
        fprintf(stderr, "Error in open()\n");
        exit(1);
    }

    // file descriptor to the file we are writing to
    write_fd = open("output", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (write_fd == -1) {
        fprintf(stderr, "Error in open()\n");
        exit(1);
    }


    // Create threads
    res = pthread_create(&write_thread, NULL, thread_write, &read_fd);
    if (res != 0) {
        fprintf(stderr, "Error in pthread_create()\n");
        exit(1);
    }

    res = pthread_create(&read_thread, NULL, thread_read, &write_fd);
    if (res != 0) {
        fprintf(stderr, "Error in pthread_create()\n");
        exit(1);
    }

    // wait until both threads have finished
    while (read_pos != -1) {}

    return 0;
}

/*
 * this function will be used by the thread that
 * is responsible for writing into the pipe
 */
void *thread_write(void *arg) {

    int fd = *(int *) arg;
    char c;
    
    lseek(fd, 0, SEEK_SET);

    while (read(fd, &c, sizeof(char))) {
        
        while (pipe_array[write_pos] != '\0') {}

        pipe_write(c);
    }

    pipe_writeDone();

    return NULL;
}

/*
 * this function will be used by the thread that
 * is responsible for reading from the pipe
 */
void *thread_read(void *arg) {

    int fd, res;
    char c;

    fd  = *(int *) arg;

    while(1) {

        while (pipe_array[read_pos] == '\0' && write_pos != -1) {}

        res = pipe_read(&c);
        if (res == 1) {
            write(fd, &c, sizeof(char));
        }
        else {
            break;
        }
    }

    return NULL;
}

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

typedef struct {
    int is_available;       // 1: if the worker is available to take a job, 0: otherwise
	int ready_to_process;   // Signal from main thread when it gives a job to the worker
                            // -1: signal from main to terminate
    int num_to_process;     // an integer to process
    int terminating;        // 1: when thread is about to terminate
} worker_t;

int isPrime(int number);
void *worker_thread(void *arg);

volatile worker_t *workers;

int main(int argc, char const *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments!\n");
        fprintf(stderr, "usage: ./prime <int>\n");
        exit(1);
    }

    int n, res;

    n = atoi(argv[1]);
    workers = calloc(n, sizeof(worker_t));
    if (workers == NULL) {
        fprintf(stderr, "Error in calloc()\n");
        exit(1);
    }

    pthread_t threads[n];

    // create workers
    for (int i = 0; i < n; i++) {

        workers[i].is_available = 0;
        workers[i].ready_to_process = 0;
        workers[i].num_to_process = 0;
        workers[i].terminating = 0;

        res = pthread_create(&threads[i], NULL, worker_thread, (void*)(intptr_t) i);
        if (res != 0) {
            fprintf(stderr, "Error in pthread_create()\n");
            exit(1);
        }
    }

    int next_int, pos = 0;

    // Timer start
    // clock_t begin = clock();

    // give jobs to workers
    while (scanf("%d", &next_int) != EOF) {

        while (workers[pos].is_available == 0) {

            if (pos == (n - 1)) {
                pos = 0;
            } else {
                pos++;
            }
        }

        workers[pos].num_to_process = (intptr_t) next_int;
        workers[pos].is_available = 0;
        workers[pos].ready_to_process = 1;
    }

    // wait for every worker to become available
    for (int i = 0; i < n; i++) {

        while (workers[i].is_available == 0) {
            pthread_yield();
        }

        // notify worker to terminate
        workers[i].ready_to_process = -1;
    }

    // wait for all workers to terminate
    for (int i = 0; i < n; i++) {

        while (workers[i].terminating == 0) {
            pthread_yield();
        }
    }

    // Timer ends
    // clock_t stop = clock();
    // double time_spent = (double) (stop - begin) / CLOCKS_PER_SEC;

    fprintf(stderr, "\nAll threads are terminated.\n");
    // fprintf(stderr, "Time spent: %f\n", time_spent);
    fprintf(stderr, "Bye!\n\n");

    free((void *) workers);

	return 0;
}

void *worker_thread(void *arg) {
	
    int index = (intptr_t) arg;
	
	while (1) {

        workers[index].is_available = 1;
        
        // wait until main thread gives a job
        while (workers[index].ready_to_process == 0) {
            pthread_yield();
        }

        if (workers[index].ready_to_process == -1) {
            break;
        }

        if (isPrime(workers[index].num_to_process)) {
            fprintf(stdout, "%d is a prime number\n", workers[index].num_to_process);
        } else {
            fprintf(stdout, "%d is not a prime number\n", workers[index].num_to_process);
        }

        workers[index].ready_to_process = 0;
	}

    workers[index].terminating = 1;

    return NULL;
}

int isPrime(int number) {
    int i;

    for (i=2; i<number; i++) {
        if (number % i == 0 && i != number) return 0;
    }

    return 1;
}
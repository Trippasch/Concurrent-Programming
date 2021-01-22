#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include "ccr.h"

typedef struct {
    int is_available;
    int num_to_process;
    int job_done;
} worker_t;

worker_t *workers;
volatile int threads_with_jobs = 0;
volatile int term_sig = 0;
volatile int main_sig = 0;
volatile int jobs, finished_jobs = 0;

CCR_DECLARE(region);

int isPrime(int number);
void *worker_thread(void *arg);

int main(int argc, char const *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments!\n");
        fprintf(stderr, "usage: ./prime <int>\n");
        exit(EXIT_FAILURE);
    }

    CCR_INIT(region);

    int n, res;

    n = atoi(argv[1]);
    workers = calloc(n, sizeof(worker_t));
    if (!workers) {
        fprintf(stderr, "ERROR: calloc(), returned NULL\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[n];

    for (int i = 0; i < n; i++) {

        workers[i].is_available = 0;
        workers[i].num_to_process = -1;
        workers[i].job_done = 0;

        res = pthread_create(&threads[i], NULL, worker_thread, (void*)(intptr_t) i);
        if (res) {
            fprintf(stderr, "Error in pthread_create(), returned %d\n", res);
            exit(EXIT_FAILURE);
        }
    }
    
    // Give job to workers
    int next_int;
    while (scanf("%d", &next_int) != EOF) {
        jobs++;
        CCR_DO(region, (threads_with_jobs < n), );
        for (int i = 0; i < n; i++) {
            if (workers[i].is_available) {
                workers[i].is_available = 0;
                workers[i].num_to_process = (intptr_t) next_int;
                threads_with_jobs++;
                break;
            }
        }
    }

    CCR_DO(region, (jobs == finished_jobs), term_sig = 1;);

    for (int i = 0; i < n; i++) {

        CCR_DO(region, (main_sig == 1), );
    }
    
    free(workers);

    return 0;
}

void *worker_thread(void *arg) {

    int index = (intptr_t) arg;

    while (1) {

        workers[index].is_available = 1;

        CCR_DO(region, (workers[index].num_to_process > 0 || term_sig == 1), );

        if (term_sig == 1 && workers[index].job_done == 1) {
            break;
        }

        if (isPrime(workers[index].num_to_process)) {
            fprintf(stdout, "%d is a prime number\n", workers[index].num_to_process);
        } else {
            fprintf(stdout, "%d is not a prime number\n", workers[index].num_to_process);
        }

        workers[index].num_to_process = -1;
        workers[index].job_done = 1;
        CCR_DO(region, 1, finished_jobs++;threads_with_jobs--;);

    }

    main_sig = 1;

    return NULL;
}

int isPrime(int number) {
    int i;

    for (i=2; i<number; i++) {
        if (number % i == 0 && i != number) return 0;
    }

    return 1;
}
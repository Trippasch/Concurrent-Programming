#define _GNU_SOURCE

#include "../assignment1/mysem.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

typedef struct {
    int num_to_process;     // an integer to process
    int job_done;
    int sem_id;             // semaphore id for this thread
} worker_t;

int isPrime(int number);
void *worker_thread(void *arg);

volatile worker_t *workers;
volatile int term_sig = 0;
int main_sem;

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

    // Create workers
    for (int i = 0; i < n; i++) {

        mysem_create(&(workers[i].sem_id), 0);
        workers[i].num_to_process = 0;
        workers[i].job_done = 0;

        res = pthread_create(&threads[i], NULL, worker_thread, (void*)(intptr_t) i);
        if (res != 0) {
            fprintf(stderr, "Error in pthread_create()\n");
            exit(1);
        }
    }

    mysem_create(&main_sem, 0);
    int next_int, pos = 0;

    // Read input and give jobs to workers
    while (scanf("%d\n", &next_int) != EOF) {

        while (!is_sleeping(workers[pos].sem_id)) {

            if (pos == (n - 1)) {
                pos = 0;
            } else {
                pos++;
            }
        }

        workers[pos].num_to_process = (intptr_t) next_int;
        workers[pos].job_done = 0;
        mysem_up(workers[pos].sem_id, __LINE__);
    }

    term_sig = 1;

    // Wait for all workers to finish their jobs and terminate
    for (int i = 0; i < n; i++) {

        mysem_up(workers[i].sem_id, __LINE__);
        mysem_down(main_sem);
        mysem_destroy(workers[i].sem_id);
    }

    mysem_destroy(main_sem);
    free((void*) workers);

    return 0;
}

void *worker_thread(void *arg) {

    int index = (intptr_t) arg;

    while (1) {

        mysem_down(workers[index].sem_id);

        if (term_sig == 1 && workers[index].job_done == 1) {
            break;
        }

        if (isPrime(workers[index].num_to_process)) {
            fprintf(stdout, "%d is a prime number\n", workers[index].num_to_process);
        } else {
            fprintf(stdout, "%d is not a prime number\n", workers[index].num_to_process);
        }

        workers[index].job_done = 1;
    }

    mysem_up(main_sem, __LINE__);

    return NULL;
}

int isPrime(int number) {
    int i;

    for (i=2; i<number; i++) {
        if (number % i == 0 && i != number) return 0;
    }

    return 1;
}

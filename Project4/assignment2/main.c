#include "mythreads.h"
#include "mylist.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define JOBS 10

typedef struct data {
    /* data */
    int num;
    int is_checked;
} data_t;

thr_t **workers = NULL;
sem_t *sem = NULL;
int thread_num = 0;
volatile data_t data[JOBS];

void worker_thr(void *arg);
int isPrime(int number);

int main(int argc, char const *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Wrong number of arguments!\n");
        fprintf(stderr, "usage: ./prime <int>\n");
        exit(EXIT_FAILURE);
    }
    
    thread_num = atoi(argv[1]);
    workers = calloc(thread_num, sizeof(thr_t));

    for (int i = 0; i < thread_num; i++) {
        workers[i] = (thr_t *) malloc(sizeof(thr_t));
        if (!workers[i]) {
            fprintf(stderr, "main: error in malloc(), line %d\n", __LINE__);
            free(workers);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < JOBS; i++) {
        scanf("%d\n", &(data[i].num));
        data[i].is_checked = 0;
    }

    sem = (sem_t *) malloc(sizeof(sem_t));
    if (!sem) {
        fprintf(stderr, "main: error in malloc(), line %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    mythreads_init();
    mythreads_sem_init(sem, 1);

    // Create workers.
    for (int i = 0; i < thread_num; i++) {
        mythreads_create(workers[i], (void (*)(void *)) worker_thr, (void *)(intptr_t) i);
    }

    mythreads_yield();

    // Main thread waits for workers to finish
    for (int i = 0; i < thread_num; i++) {
        mythreads_join(workers[i]);
    }

    // Destroy workers
    for (int i = 0; i < thread_num; i++) {
        mythreads_destroy(workers[i]);
    }

    // Destroy the semaphore
    mythreads_sem_destroy(sem);
    free(workers);

    return 0;
}

void worker_thr(void *arg) {

    int thread_id = (intptr_t) arg;
    int res, i;

    while (1) {
        mythreads_sem_down(sem);

        if (workers[thread_id]->finished == 1) {
            break;
        }

        for (i = 0; i < JOBS; i++) {
            if (!(data[i].is_checked)) {
                res = isPrime(data[i].num);
                data[i].is_checked = 1;
                break;
            }
        }

        if (res) {
            fprintf(stdout, "%d: %d is a prime number!\n", thread_id, data[i].num);
        } else {
            fprintf(stdout, "%d: %d is not a prime number!\n", thread_id, data[i].num);
        }

        if (i == JOBS - 1) {
            for (i = 0; i < thread_num; i++) {
                workers[i]->finished = 1;
            }
        }

        mythreads_sem_up(sem);
        mythreads_yield();
    }
    return;
}

int isPrime(int number) {
    int i;

    for (i=2; i<number; i++) {
        if (number % i == 0 && i != number) return 0;
    }

    return 1;
}

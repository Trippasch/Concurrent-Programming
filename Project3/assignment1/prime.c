#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

typedef struct {
    int is_available;       // 1: if the worker is available to take a job, 0: otherwise
    int num_to_process;     // an integer to process
    int ready_to_process;   // Signal from main thread when it gives a job to the worker
    int job_done;
    pthread_cond_t wait;
} worker_t;

int isPrime(int number);
void *worker_thread(void *arg);

worker_t *workers;
volatile int term_sig = 0;

pthread_mutex_t mtx;
pthread_cond_t main_wait;

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

    res = pthread_mutex_init(&mtx, NULL);
    if (res) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", res);
        exit(-1);
    }

    res = pthread_cond_init(&main_wait, NULL);
    if (res) {
        printf("ERROR: return code from pthread_cond_init() is %d\n", res);
        exit(-1);
    }

    // Create workers
    for (int i = 0; i < n; i++) {

        res = pthread_cond_init(&workers[i].wait, NULL);
        if (res) {
            printf("ERROR: return code from pthread_cond_init() is %d\n", res);
            exit(-1);
        }
        workers[i].num_to_process = 0;
        workers[i].job_done = 0;
        workers[i].ready_to_process = 0;
        workers[i].is_available = 0;

        res = pthread_create(&threads[i], NULL, worker_thread, (void*)(intptr_t) i);
        if (res != 0) {
            fprintf(stderr, "Error in pthread_create()\n");
            exit(1);
        }
    }

    int next_int, pos = 0;

    // Read input and give jobs to workers
    while (scanf("%d", &next_int) != EOF) {

        res = pthread_mutex_lock(&mtx);
        if (res) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", res);
            exit(-1);
        }

        while (workers[pos].is_available == 0) {

            if (pos == (n - 1)) {
                res = pthread_cond_wait(&main_wait, &mtx);
                if (res) {
                    printf("ERROR: return code from pthread_cond_wait() is %d\n", res);
                    exit(-1);
                }
                pos = 0;
            } else {
                pos++;
            }
        }

        workers[pos].num_to_process = (intptr_t) next_int;
        workers[pos].job_done = 0;
        workers[pos].ready_to_process = 1;
        workers[pos].is_available = 0;

        res = pthread_cond_signal(&workers[pos].wait);
        if (res) {
            printf("ERROR: return code from pthread_cond_signal() is %d\n", res);
            exit(-1);
        }

        res = pthread_mutex_unlock(&mtx);
        if (res) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", res);
            exit(-1);
        }
    }

    term_sig = 1;
    for (int i = 0; i < n; i++) {
        res = pthread_cond_signal(&workers[i].wait);
        if (res) {
            printf("ERROR: return code from pthread_cond_signal() is %d\n", res);
            exit(-1);
        }
        res = pthread_cond_wait(&main_wait, &mtx);
        if (res) {
            printf("ERROR: return code from pthread_cond_wait() is %d\n", res);
            exit(-1);
        }
        res = pthread_cond_destroy(&workers[i].wait);
        if (res) {
            printf("ERROR: return code from pthread_cond_destroy() is %d\n", res);
            exit(-1);
        }
    }

    res = pthread_mutex_destroy(&mtx);
    if (res) {
        printf("ERROR: return code from pthread_muxtex_destroy() is %d\n", res);
        exit(-1);
    }

    free(workers);

    return 0;
}

void *worker_thread(void *arg) {

    int index = (intptr_t) arg;
    int res;

    while (1) {

        workers[index].is_available = 1;

        res = pthread_mutex_lock(&mtx);
        if (res) {
            printf("ERROR: return code from pthread_mutex_lock() is %d\n", res);
            exit(-1);
        }

        while(workers[index].ready_to_process == 0) {

            if (term_sig == 1 && workers[index].job_done == 1) {
                break;
            }

            res = pthread_cond_wait(&workers[index].wait, &mtx);
            if (res) {
                printf("ERROR: return code from pthread_cond_wait() is %d\n", res);
                exit(-1);
            }
        }

        res = pthread_mutex_unlock(&mtx);
        if (res) {
            printf("ERROR: return code from pthread_mutex_unlock() is %d\n", res);
            exit(-1);
        }

        if (term_sig == 1 && workers[index].job_done == 1) {
            break;
        }

        if (isPrime(workers[index].num_to_process)) {
            fprintf(stdout, "%d is a prime number\n", workers[index].num_to_process);
        } else {
            fprintf(stdout, "%d is not a prime number\n", workers[index].num_to_process);
        }

        workers[index].job_done = 1;
        workers[index].ready_to_process = 0;

        res = pthread_cond_signal(&main_wait);
        if (res) {
            printf("ERROR: return code from pthread_cond_signal() is %d\n", res);
            exit(-1);
        }
    }

    res = pthread_cond_signal(&main_wait);
    if (res) {
        printf("ERROR: return code from pthread_cond_signal() is %d\n", res);
        exit(-1);
    }

    return NULL;
}

int isPrime(int number) {
    int i;

    for (i=2; i<number; i++) {
        if (number % i == 0 && i != number) return 0;
    }

    return 1;
}

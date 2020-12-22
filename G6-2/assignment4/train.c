#include "../assignment1/mysem.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CAPACITY 3
#define RIDE_TIME 2

typedef struct train {
    int max_cap;
    int ride_time;
    int curr_passengers;
    int total_visitors;
    int remaining_visitors;
} train_t;

void *train_thread(void *args);
void *passenger_thread(void *args);

volatile train_t *train;
volatile int main_sem, train_sem, passenger_sem, queue;
// volatile int total_visitors;

int main(int argc, char const *argv[]) {
    
    int res;
    pthread_t train_th, *passenger_th;

    mysem_create(&main_sem, 0);
    mysem_create(&train_sem, 0);
    mysem_create(&passenger_sem, 1);
    mysem_create(&queue, 0);

    train = (train_t *) malloc(sizeof(train_t));
    train->max_cap = MAX_CAPACITY;
    train->ride_time = RIDE_TIME;
    train->curr_passengers = 0;

    do {
        printf("How many visitors? ");
        scanf("%d", &train->total_visitors);

        if (train->total_visitors <= 0) {
            printf("Visitors must be possiteve!\n");
        }
    } while (train->total_visitors <= 0);

    train->remaining_visitors = train->total_visitors;

    res = pthread_create(&train_th, NULL, train_thread, NULL);
    if (res != 0) {
        fprintf(stderr, "Error in pthread_create()\n");
        exit(EXIT_FAILURE);
    }

    passenger_th = calloc(train->total_visitors, sizeof(pthread_t));
    if (passenger_th == NULL) {
        fprintf(stderr, "Error in calloc()\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < train->total_visitors; i++) {

        res = pthread_create(&passenger_th[i], NULL, passenger_thread, NULL);
        if (res != 0) {
            fprintf(stderr, "Error in pthread_create()\n");
            exit(EXIT_FAILURE);
        }
    }

    mysem_down(main_sem);

    mysem_destroy(main_sem);
    mysem_destroy(train_sem);
    mysem_destroy(queue);
    free((void *) train);

    return 0;
}

void *train_thread(void *args) {

    while (1) {
        mysem_down(train_sem);

        printf("Starting Ride!\n");
        sleep(train->ride_time);
        printf("Finished Ride!\n");

        if (train->curr_passengers <= train->max_cap &&
            train->remaining_visitors == 0) {
            break;
        }

        printf("Visitors remained: %d\n", train->remaining_visitors);

        train->curr_passengers = 0;

        mysem_up(queue, __LINE__);
    }

    mysem_up(main_sem, __LINE__);

    return NULL;
}

void *passenger_thread(void *args) {

    mysem_down(passenger_sem);

    train->curr_passengers++;
    train->remaining_visitors--;

    printf("Passengers on train: %d\n", train->curr_passengers);

    if (train->curr_passengers >= train->max_cap) {
        mysem_up(train_sem, __LINE__);
        mysem_down(queue);
    }

    if (train->remaining_visitors <= 0) {
        mysem_up(train_sem, __LINE__);
        return NULL;
    }

    mysem_up(passenger_sem, __LINE__);

    return NULL;
}

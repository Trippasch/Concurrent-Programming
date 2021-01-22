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
    int on_ride;
} train_t;

void *train_thread(void *args);
void *passenger_thread(void *args);
void print_error(char *, int);

volatile train_t *train;

pthread_mutex_t mtx;
pthread_mutex_t train_mtx;
pthread_cond_t queue;
pthread_cond_t on_board;
pthread_cond_t train_wait;
pthread_cond_t main_wait;

int main(int argc, char const *argv[]) {
    
    int res;
    pthread_t train_th, *passenger_th;

    train = (train_t *) malloc(sizeof(train_t));
    train->max_cap = MAX_CAPACITY;
    train->ride_time = RIDE_TIME;
    train->curr_passengers = 0;
    train->on_ride = 0;

    res = pthread_mutex_init(&mtx, NULL);
    if (res) print_error("pthread_mutex_init()", res);

    res = pthread_mutex_init(&train_mtx, NULL);
    if (res) print_error("pthread_mutex_init()", res);

    res = pthread_cond_init(&queue, NULL);
    if (res) print_error("pthread_cond_init()", res);

    res = pthread_cond_init(&on_board, NULL);
    if (res) print_error("pthread_cond_init()", res);

    res = pthread_cond_init(&train_wait, NULL);
    if (res) print_error("pthread_cond_init()", res);

    res = pthread_cond_init(&main_wait, NULL);
    if (res) print_error("pthread_cond_init()", res);

    do {
        printf("How many visitors? ");
        scanf("%d", &train->total_visitors);

        if (train->total_visitors <= 0) {
            printf("Visitors must be possiteve!\n");
        }
    } while (train->total_visitors <= 0);

    train->remaining_visitors = train->total_visitors;

    res = pthread_create(&train_th, NULL, train_thread, NULL);
    if (res) print_error("pthread_create()", res);

    passenger_th = calloc(train->total_visitors, sizeof(pthread_t));
    if (passenger_th == NULL) print_error("calloc()", -1);

    for (int i = 0; i < train->total_visitors; i++) {

        res = pthread_create(&passenger_th[i], NULL, passenger_thread, NULL);
        if (res) print_error("pthread_create()", res);
    }

    res = pthread_mutex_lock(&mtx);
    if (res) print_error("pthread_mutex_lock()", res);

    res = pthread_cond_wait(&main_wait, &mtx);
    if (res) print_error("pthread_cond_wait()", res);

    res = pthread_mutex_unlock(&mtx);
    if (res) print_error("pthread_mutex_unlock()", res);

    res = pthread_mutex_destroy(&mtx);
    if (res) print_error("pthread_mutex_destroy(&mtx)", res);

    res = pthread_mutex_destroy(&train_mtx);
    if (res) print_error("pthread_mutex_destroy(&train_mtx)", res);

    res = pthread_cond_destroy(&queue);
    if (res) print_error("pthread_cond_destroy()", res);

    res = pthread_cond_destroy(&on_board);
    if (res) print_error("pthread_cond_destroy()", res);

    res = pthread_cond_destroy(&train_wait);
    if (res) print_error("pthread_cond_destroy()", res);

    res = pthread_cond_destroy(&main_wait);
    if (res) print_error("pthread_cond_destroy()", res);

    free((void *) train);

    return 0;
}

void *train_thread(void *args) {

    int res;

    while (1) {

        res = pthread_mutex_lock(&train_mtx);
        if (res) print_error("pthread_mutex_lock()", res);

        res = pthread_cond_wait(&train_wait, &train_mtx);
        if (res) print_error("pthread_cond_wait()", res);

        if (train->remaining_visitors <= 0 && train->on_ride == 0) {
            break;
        }

        printf("Starting Ride!\n");
        sleep(train->ride_time);
        printf("Finished Ride!\n");

        printf("Visitors remained: %d\n", train->remaining_visitors);

        res = pthread_cond_signal(&on_board);
        if (res) print_error("pthread_cond_signal()", res);

        res = pthread_mutex_unlock(&train_mtx);
        if (res) print_error("pthread_mutex_unlock()", res);
    }

    res = pthread_cond_signal(&main_wait);
    if (res) print_error("pthread_cond_signal()", res);

    res = pthread_mutex_unlock(&train_mtx);
    if (res) print_error("pthread_mutex_unlock()", res);

    return NULL;
}

void *passenger_thread(void *args) {

    int res;

    res = pthread_mutex_lock(&mtx);
    if (res) print_error("pthread_mutex_lock()", res);

    while ((train->curr_passengers >= train->max_cap) || train->on_ride) {
        res = pthread_cond_wait(&queue, &mtx);
        if (res) print_error("pthread_cond_wait()", res);
    }

    train->curr_passengers++;
    train->remaining_visitors--;

    printf("Passengers on train: %d\n", train->curr_passengers);

    if (train->curr_passengers >= train->max_cap || train->remaining_visitors == 0) {
        train->on_ride = 1;
        res = pthread_cond_signal(&train_wait);
        if (res) print_error("pthread_cond_signal()", res);
    }

    res = pthread_cond_signal(&queue);
    if (res) print_error("pthread_cond_signal()", res);
    res = pthread_cond_wait(&on_board, &mtx);
    if (res) print_error("pthread_cond_wait()", res);

    train->curr_passengers--;
    printf("Passenger left!\n");

    if (train->curr_passengers > 0) {
        res = pthread_cond_signal(&on_board);
        if (res) print_error("pthread_cond_signal()", res);
    }
    else {
        train->on_ride = 0;
        res = pthread_cond_signal(&queue);
        if (res) print_error("pthread_cond_signal()", res);
    }

    if (train->remaining_visitors <= 0) {
        train->on_ride = 0;
        res = pthread_cond_signal(&train_wait);
        if (res) print_error("pthread_cond_signal()", res);
    }

    res = pthread_mutex_unlock(&mtx);
    if (res) print_error("pthread_mutex_unlock()", res);

    return NULL;
}

void print_error(char *func, int res) {

    fprintf(stderr, "ERROR: return code from %s is %d\n", func, res);
    exit(EXIT_FAILURE);
}
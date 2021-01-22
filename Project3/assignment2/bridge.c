#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LEFT_TO_RIGHT 0
#define RIGHT_TO_LEFT 1
#define MAX_CAPACITY  3

#define BRED "\x1B[1;31m"
#define BBLUE "\x1B[1;34m"
#define RESET "\x1B[0m"

typedef struct {
    int N;
    int direction;
    int num_of_cars;
    int red_q, blue_q;
    int counter;
    int wait_empty;
} bridge_state_t;

typedef struct {
    int direction;
    int drive_time;
} car_info_t;

volatile bridge_state_t bridge_state;
volatile int generated_cars = 0;

pthread_mutex_t enter_mtx, leave_mtx, main_mtx;
pthread_cond_t blue_q, red_q, main_wait;

void bridge_enter(int);
void bridge_leave(int);
void *car_thread(void *);

int main(int argc, char *argv[]) {

    bridge_state.N = MAX_CAPACITY;
    bridge_state.direction = 0;
    bridge_state.num_of_cars = 0;
    bridge_state.red_q = 0;
    bridge_state.blue_q = 0;
    bridge_state.counter = 0;
    bridge_state.wait_empty = 0;

    int res, delay;
    car_info_t *car;
    pthread_t thread;

    res = pthread_mutex_init(&enter_mtx, NULL);
    if (res) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", res);
        exit(-1);
    }

    res = pthread_mutex_init(&leave_mtx, NULL);
    if (res) {
        printf("ERROR: return code from pthread_mutex_init() is %d\n", res);
        exit(-1);
    }

    res = pthread_cond_init(&blue_q, NULL);
    if (res) {
        printf("ERROR: return code from pthread_cond_init() is %d\n", res);
        exit(-1);
    }

    res = pthread_cond_init(&red_q, NULL);
    if (res) {
        printf("ERROR: return code from pthread_cond_init() is %d\n", res);
        exit(-1);
    }

    res = pthread_cond_init(&main_wait, NULL);
    if (res) {
        printf("ERROR: return code from pthread_cond_init() is %d\n", res);
        exit(-1);
    }

    while(1) {
        car = (car_info_t *) malloc(sizeof(car_info_t));
        if (car == NULL) {
            fprintf(stderr, "Error in malloc()\n");
            exit(EXIT_FAILURE);
        }
        scanf("%d %d %d", &car->direction, &car->drive_time, &delay);
        if (car->direction == -1) {
            break;
        }
        else if (car->direction != LEFT_TO_RIGHT && car->direction != RIGHT_TO_LEFT) {
            printf("Wrong Input!\n");
            continue;
        }

        generated_cars++;
        sleep(delay);

        res = pthread_create(&thread, NULL, car_thread, (void *) car);
        if (res != 0) {
            fprintf(stderr, "Error in pthread_create()\n");
            exit(EXIT_FAILURE);
        }
    }

    res = pthread_mutex_lock(&main_mtx);
    if (res) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", res);
        exit(-1);
    }

    res = pthread_cond_wait(&main_wait, &main_mtx);
    if (res) {
        printf("ERROR: return code from pthread_cond_wait() is %d\n", res);
        exit(-1);
    }

    res = pthread_mutex_unlock(&main_mtx);
    if (res) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", res);
        exit(-1);
    }

    res = pthread_mutex_destroy(&enter_mtx);
    if (res) {
        printf("ERROR: return code from pthread_muxtex_destroy() is %d\n", res);
        exit(-1);
    }

    res = pthread_mutex_destroy(&leave_mtx);
    if (res) {
        printf("ERROR: return code from pthread_muxtex_destroy() is %d\n", res);
        exit(-1);
    }

    res = pthread_cond_destroy(&blue_q);
    if (res) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", res);
        exit(-1);
    }

    res = pthread_cond_destroy(&red_q);
    if (res) {
        printf("ERROR: return code from pthread_cond_destroy() is %d\n", res);
        exit(-1);
    }

    return 0;
}

void *car_thread(void *args) {

    car_info_t *car = (car_info_t *) args;

    bridge_enter(car->direction);
    sleep(car->drive_time);
    bridge_leave(car->direction);

    free(car);

    return NULL;
}

void bridge_enter(int direction) {

    int res;

    res = pthread_mutex_lock(&enter_mtx);
    if (res) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", res);
        exit(-1);
    }

    if (direction) {
        bridge_state.blue_q++;
    }
    else {
        bridge_state.red_q++;
    }

    while ((bridge_state.num_of_cars > 0 &&
        (bridge_state.num_of_cars == bridge_state.N
        || bridge_state.direction != direction)) || bridge_state.wait_empty) {

        if (bridge_state.num_of_cars == bridge_state.N) {
            printf("Bridge is full!\n");
        }
        else if (bridge_state.direction != direction) {
            printf("Bridge is used by the other direction!\n");
        }
        else {
            printf("Waiting for the bridge to empty!\n");
        }

        if (direction) {
            res = pthread_cond_wait(&blue_q, &enter_mtx);
            
        }
        else {
            res = pthread_cond_wait(&red_q, &enter_mtx);
        }
        if (res) {
            printf("ERROR: return code from pthread_cond_wait() is %d\n", res);
            exit(-1);
        }
    }

    if (bridge_state.num_of_cars == 0 && bridge_state.direction != direction) {
        bridge_state.direction = direction;
        if (direction == LEFT_TO_RIGHT) {
            printf(BRED "Bridge direction is left to right!\n" RESET);
        }
        else {
            printf(BBLUE "Bridge direction is right to left!\n" RESET);
        }
    }

    bridge_state.num_of_cars++;
    bridge_state.counter++;

    if (direction) {
        printf(BBLUE "Blue car entered!\n" RESET);
        if (bridge_state.blue_q > 0) {
            bridge_state.blue_q--;
        }
        // printf("Blue queue = %d\n", bridge_state.blue_q);
    }
    else {
        printf(BRED "Red car entered!\n" RESET);
        if (bridge_state.red_q > 0) {
            bridge_state.red_q--;
        }
        // printf("Red queue = %d\n", bridge_state.red_q);
    }

    if (bridge_state.counter == MAX_CAPACITY) {
        if (direction && bridge_state.red_q) {
            bridge_state.direction = !bridge_state.direction;
            bridge_state.wait_empty = 1;
            printf(BRED "Bridge direction changed!\n" RESET);
        }
        else if (!direction && bridge_state.blue_q) {
            bridge_state.direction = !bridge_state.direction;
            bridge_state.wait_empty = 1;
            printf(BBLUE "Bridge direction changed!\n" RESET);
        }
        bridge_state.counter = 0;
        
    }

    if (bridge_state.num_of_cars < bridge_state.N) {
        if (direction) {
            res = pthread_cond_signal(&blue_q);
        }
        else {
            res = pthread_cond_signal(&red_q);
        }
        if (res) {
            printf("ERROR: return code from pthread_cond_signal() is %d\n", res);
            exit(-1);
        }
    }

    res = pthread_mutex_unlock(&enter_mtx);
    if (res) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", res);
        exit(-1);
    }

}

void bridge_leave(int direction) {

    int res;

    res = pthread_mutex_lock(&leave_mtx);
    if (res) {
        printf("ERROR: return code from pthread_mutex_lock() is %d\n", res);
        exit(-1);
    }

    bridge_state.num_of_cars--;
    generated_cars--;

    if (direction) {
        printf(BBLUE "Blue car left the bridge\n" RESET);
    }
    else {
        printf(BRED "Red car left the bridge\n" RESET);
    }

    if (bridge_state.wait_empty && bridge_state.num_of_cars == 0) {
        bridge_state.wait_empty = 0;
    }

    if (bridge_state.direction && bridge_state.blue_q) {
        res = pthread_cond_signal(&blue_q);
    }
    else if (bridge_state.direction && !bridge_state.blue_q){
        res = pthread_cond_signal(&red_q);
    }
    else if (!bridge_state.direction && bridge_state.red_q){
        res = pthread_cond_signal(&red_q);
    }
    else if (!bridge_state.direction && !bridge_state.red_q){
        res = pthread_cond_signal(&blue_q);
    }

    if (res) {
        printf("ERROR: return code from pthread_cond_signal() is %d\n", res);
        exit(-1);
    }

    if (generated_cars == 0) {
        res = pthread_cond_signal(&main_wait);
        if (res) {
            printf("ERROR: return code from pthread_cond_signal() is %d\n", res);
            exit(-1);
        }
    }

    res = pthread_mutex_unlock(&leave_mtx);
    if (res) {
        printf("ERROR: return code from pthread_mutex_unlock() is %d\n", res);
        exit(-1);
    }

}

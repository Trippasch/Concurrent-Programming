#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "../assignment1/mysem.h"

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
} bridge_state_t;

typedef struct {
    int direction;
    int drive_time;
} car_info_t;

volatile bridge_state_t bridge_state;
volatile int main_sem, bridge_enter_sem, bridge_leave_sem, wait_sem;
volatile int generated_cars = 0;

void bridge_enter(int, int);
void bridge_leave(int, int);
void *car_thread(void *);

int main(int argc, char *argv[]) {

    bridge_state.N = MAX_CAPACITY;
    bridge_state.direction = 0;
    bridge_state.num_of_cars = 0;
    bridge_state.red_q = 0;
    bridge_state.blue_q = 0;

    int res, delay;
    car_info_t *car;
    pthread_t thread;

    mysem_create(&main_sem, 0);
    mysem_create(&bridge_enter_sem, 1);
    mysem_create(&bridge_leave_sem, 1);
    mysem_create(&wait_sem, 0);

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

    mysem_down(main_sem);

    mysem_destroy(main_sem);
    mysem_destroy(bridge_enter_sem);
    mysem_destroy(bridge_leave_sem);
    mysem_destroy(wait_sem);

    return 0;
}

void *car_thread(void *args) {

    car_info_t *car = (car_info_t *) args;

    bridge_enter(car->direction, car->drive_time);
    sleep(car->drive_time);
    bridge_leave(car->direction, car->drive_time);

    free(car);

    return NULL;
}

void bridge_enter(int direction, int drive_time) {

    mysem_down(bridge_enter_sem);

    if (direction) {
        bridge_state.blue_q++;
    }
    else {
        bridge_state.red_q++;
    }

    while (bridge_state.num_of_cars > 0 &&
        (bridge_state.num_of_cars == bridge_state.N
        || bridge_state.direction != direction)) {

        if (bridge_state.num_of_cars == bridge_state.N) {
            printf("Bridge is full!\n");
        }
        else {
            printf("Bridge is used by the other direction!\n");
        }

        mysem_up(bridge_enter_sem, __LINE__);
        mysem_down(wait_sem);
        mysem_down(bridge_enter_sem);
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

    mysem_up(bridge_enter_sem, __LINE__);
}

void bridge_leave(int direction, int drive_time) {

    mysem_down(bridge_leave_sem);

    bridge_state.num_of_cars--;
    generated_cars--;

    if (direction) {
        printf(BBLUE "Blue car left the bridge\n" RESET);
    }
    else {
        printf(BRED "Red car left the bridge\n" RESET);
    }

    if (bridge_state.red_q > 0 || bridge_state.blue_q > 0) {
        mysem_up(wait_sem, __LINE__);
    }

    if (generated_cars == 0) {
        mysem_up(main_sem, __LINE__);
    }

    mysem_up(bridge_leave_sem, __LINE__);
}

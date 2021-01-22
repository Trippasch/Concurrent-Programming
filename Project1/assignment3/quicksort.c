#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// size of the array to sort
#define SIZE 20

typedef struct {
    int left;           // left edge of the array to sort
    int right;          // right edge of the array to sort
    int finished;       // 1: when thread has finished, 0: otherwise
} info_t;

// Quick Sort function ran by threads
void *quick_sort(void *args);
// Function to sort a section of the array
int sort_section(int left, int right);
// swap two elements of the array
void swap(volatile int *a, volatile int *b);
void print_array();

// Array to sort
volatile int *array;

int main(int argc, char const *argv[]) {

    info_t *info;

    srand(time(NULL));
    // srand(1);                // use this for debuging

    // allocate memory for the array to sort
    array = (int *) calloc(SIZE, sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Error in calloc()\n");
        exit(1);
    }

    // initialize the array with random numbers
    for (int i = 0; i < SIZE; i++) {
        array[i] = (rand() % SIZE) + 1;
    }

    printf("\n------------ Unsorted Array ------------\n");
    print_array();

    info = (info_t *) malloc(sizeof(info_t));
    if (info == NULL) {
        fprintf(stderr, "Error in malloc()\n");
        exit(1);
    }

    info->left = 0;
    info->right = SIZE - 1;
    info->finished = 0;

    quick_sort(info);

    printf("\n------------- Sorted Array -------------\n");
    print_array();
    
    return 0;
}

void *quick_sort(void *args) {

    info_t *self = (info_t *) args;

    if (self->right < self->left) {

        self->finished = 1;
        return NULL;
    }

    // Sort current section and get delimiter
    int del = sort_section(self->left, self->right);

    int res;
    pthread_t left_t, right_t;
    info_t *left_sec, *right_sec;

    // Create threads to handle left and right sections of the array
    left_sec = (info_t *) malloc(sizeof(info_t));
    if (left_sec == NULL) {
        fprintf(stderr, "Error in malloc()\n");
        exit(1);
    }

    left_sec->left = self->left;
    left_sec->right = del - 1;
    left_sec->finished = 0;

    res = pthread_create(&left_t, NULL, quick_sort, (void *) left_sec);
    if (res != 0) {
        fprintf(stderr, "Error in pthread_create()\n");
        exit(1);
    }

    right_sec = (info_t *) malloc(sizeof(info_t));
    if (right_sec == NULL) {
        fprintf(stderr, "Error in malloc()\n");
        exit(1);
    }

    right_sec->left = del + 1;
    right_sec->right = self->right;
    right_sec->finished = 0;

    res = pthread_create(&right_t, NULL, quick_sort, (void *) right_sec);
    if (res != 0) {
        fprintf(stderr, "Error in pthread_create()\n");
        exit(1);
    }

    while (right_sec->finished != 1 || left_sec->finished != 1) {
        pthread_yield();
    }

    free(left_sec);
    free(right_sec);
    self->finished = 1;

    return NULL;
}

int sort_section(int left, int right) {

    int pivot = array[right];
    int i = (left - 1);

    for (int j = left; j <= right - 1; j++) {

        if (array[j] < pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }

    swap(&array[i + 1], &array[right]);
    return (i + 1);
}

void swap(volatile int *a, volatile int *b) {

    int t = *a;
    *a = *b;
    *b = t;
}

void print_array() {

    for (int i = 0; i < SIZE; i++) {
        printf("%d ", array[i]);
    }

    putchar('\n');
}

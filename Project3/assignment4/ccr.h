#ifndef __CCR_H__
#define __CCR_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct region {
    pthread_mutex_t R_mtx;
    pthread_cond_t  R_q1;
    pthread_cond_t  R_q2;
    int R_n1;
    int R_n2;
} region_t;

#define CCR_DECLARE(label) region_t label;

#define CCR_INIT(label) ({\
    if (pthread_mutex_init(&label.R_mtx, NULL)) {\
        printf("ERROR: pthread_mutex_init()\n");\
        exit(EXIT_FAILURE);\
    }\
    if (pthread_cond_init(&label.R_q1, NULL)) {\
        printf("ERROR: pthread_cond_init()\n");\
        exit(EXIT_FAILURE);\
    }\
    if (pthread_cond_init(&label.R_q2, NULL)) {\
        printf("ERROR: pthread_cond_init()\n");\
        exit(EXIT_FAILURE);\
    }\
    label.R_n1 = label.R_n2 = 0;\
})

#define CCR_DO(label, lexpr, body) ({\
    if (pthread_mutex_lock(&label.R_mtx)) {\
        printf("ERROR: pthread_mutex_lock()\n");\
        exit(EXIT_FAILURE);\
    }\
    while (!lexpr) {\
        label.R_n1++;\
        if (label.R_n2 > 0) {\
            label.R_n2--;\
            if (pthread_cond_signal(&label.R_q2)) {\
                printf("ERROR: pthread_cond_signal()\n");\
                exit(EXIT_FAILURE);\
            }\
        }\
        if (pthread_cond_wait(&label.R_q1, &label.R_mtx)) {\
            printf("ERROR: pthread_cond_wait()\n");\
            exit(EXIT_FAILURE);\
        }\
        label.R_n2++;\
        if (label.R_n1 > 0) {\
            label.R_n1--;\
            if (pthread_cond_signal(&label.R_q1)) {\
                printf("ERROR: pthread_cond_signal()\n");\
                exit(EXIT_FAILURE);\
            }\
        } else if (label.R_n2 >= 2){\
            label.R_n2--;\
            if (pthread_cond_signal(&label.R_q2)) {\
                printf("ERROR: pthread_cond_signal()\n");\
                exit(EXIT_FAILURE);\
            }\
        }\
        if (pthread_cond_wait(&label.R_q2, &label.R_mtx)) {\
            printf("ERROR: pthread_cond_wait()\n");\
            exit(EXIT_FAILURE);\
        }\
    }\
    body;\
    if (label.R_n1 > 0) {\
        label.R_n1--;\
        if (pthread_cond_signal(&label.R_q1)) {\
            printf("ERROR: pthread_cond_signal()\n");\
            exit(EXIT_FAILURE);\
        }\
    } else if (label.R_n2 > 0) {\
        label.R_n2--;\
        if (pthread_cond_signal(&label.R_q2)) {\
            printf("ERROR: pthread_cond_signal()\n");\
            exit(EXIT_FAILURE);\
        }\
    }\
    if (pthread_mutex_unlock(&label.R_mtx)) {\
        printf("ERROR: pthread_mutex_unlock()\n");\
        exit(EXIT_FAILURE);\
    }\
})

#endif
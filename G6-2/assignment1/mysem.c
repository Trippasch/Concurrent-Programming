#define _GNU_SOURCE
#include "mysem.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <errno.h>

struct sembuf sops;

void mysem_create(volatile int *sem_id, int init_num) {

    *sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0664);
    if (*sem_id == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    if (semctl(*sem_id, 0, SETVAL, init_num) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
}

void mysem_down(int sem_id) {

    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    if (semop(sem_id, &sops, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }
}

int mysem_up(int sem_id, const int line) {

    int val;

    val = semctl(sem_id, 0, GETVAL);

    if (val == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    } else if (val == 1) {
        fprintf(stderr, "Lost call of mysem_up() at line: %d\n", line);
        return -1;
    }

    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = 0;

    if (semop(sem_id, &sops, 1) == -1) {
        perror("semop");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void mysem_destroy(int sem_id) {

    if (semctl(sem_id, 0, IPC_RMID) == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }
}

int is_sleeping(int sem_id) {

    int val;

    val = semctl(sem_id, 0, GETNCNT);
    if (val == -1) {
        perror("semctl");
        exit(EXIT_FAILURE);
    }

    // fprintf(stderr, "Val = %d\n", val);
    return val;
}

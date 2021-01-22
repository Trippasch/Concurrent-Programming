#ifndef __MY_THREADS_H__
#define __MY_THREADS_H__

#include <ucontext.h>
#include <sys/sem.h>
#include <signal.h>

typedef struct {
    /* sem data */
    int value;
    int is_blocked;
} sem_t;

typedef struct {
    /* thread data */
    int key;
    int finished;
    ucontext_t context;
    char stack[SIGSTKSZ];
} thr_t;

int mythreads_init();
int mythreads_create(thr_t *thr, void (body)(void *), void *arg);
int mythreads_yield();
int mythreads_join(thr_t *thr);
int mythreads_destroy(thr_t *thr);

int mythreads_sem_init(sem_t *s, int val);
int mythreads_sem_down(sem_t *s);
int mythreads_sem_up(sem_t *s);
int mythreads_sem_destroy(sem_t *s);

void handler();
void scheduler();

#endif
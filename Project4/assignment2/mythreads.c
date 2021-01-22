#include "mythreads.h"
#include "mylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <ucontext.h>

ucontext_t main_context;

int keys = 0;
list_t *list;
sem_t *sem;

int mythreads_init() {

    getcontext(&main_context);
    list = mylist_init();

    signal(SIGALRM, handler);

    return 0;
}

int mythreads_create(thr_t *thr, void (*body)(void *), void *arg) {

    getcontext(&(thr->context));
    thr->key = keys++;
    thr->finished = 0;
    thr->context.uc_stack.ss_sp = thr->stack;
    thr->context.uc_stack.ss_size = sizeof(thr->stack);
    thr->context.uc_link = &main_context;

    makecontext(&thr->context, (void (*)(void))body, 1, arg);

    mylist_add(list, thr->key, thr);

    return 0;
}

int mythreads_yield() {
    scheduler();
    return 0;
}

int mythreads_join(thr_t *thr) {

    while (!(thr->finished)) {}

    return 0;
}

int mythreads_destroy(thr_t *thr) {

    free(thr);

    return 0;
}

int mythreads_sem_init(sem_t *s, int val) {
    
    if (val < 0) {
        fprintf(stderr, "mythreads_sem_init: value must be non-negative\n");
        exit(EXIT_FAILURE);
    }

    s->value = val;
    s->is_blocked = 0;

    sem = s;

    return 0;
}

int mythreads_sem_down(sem_t *s) {

    if (s->value > 0) {
        (s->value)--;
    } else {
        s->is_blocked = 1;
        scheduler();
    }
    
    return 0;
}

int mythreads_sem_up(sem_t *s) {

    s->is_blocked = 0;
    (s->value)++;

    return 0;
}

int mythreads_sem_destroy(sem_t *s) {

    free(s);

    return 0;
}

void handler() {
    // fprintf(stderr, "handler triggered from SIGALRM\n");
    if (!(sem->is_blocked)) {
        scheduler();
    }
}

void scheduler() {
    if (list->active_process != NULL) {
        fprintf(stderr, "rescheduling...\n");
        list->active_process = list->active_process->next;
        if (list->active_process == list->head) {
            list->active_process = list->active_process->next;
        }
        alarm(2);
        setcontext(&(list->active_process->thread->context));
    }
    alarm(2);
}

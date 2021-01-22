#ifndef __MY_COROUTINES_H__
#define __MY_COROUTINES_H__

typedef struct{
    ucontext_t context;
    char stack[SIGSTKSZ];
}co_t;

extern ucontext_t main_context;
extern ucontext_t *current_context;

int mycoroutines_init(co_t *main);
int mycoroutines_create(co_t *co, void(body)(void*), void *arg);
int mycoroutines_switchto(co_t *co);
int mycoroutines_destroy(co_t *co);

#endif
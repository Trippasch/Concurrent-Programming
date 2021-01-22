#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include "mycoroutines.h"

ucontext_t main_context;
ucontext_t *current_context;

int mycoroutines_init(co_t *main) {

    getcontext(&main->context);
    main_context = main->context;

    return 0;
}

int mycoroutines_create(co_t *co, void(body)(void*), void *arg) {

    getcontext(&co->context);                           // init internal stuff
    co->context.uc_stack.ss_sp = co->stack;             // the stack for this context
    co->context.uc_stack.ss_size = sizeof(co->stack);   // the stack size
    co->context.uc_link = &main_context;                // to switch when exiting this context

    // init stack & code
    makecontext(&co->context, (void (*)(void)) body, 1, arg);

    return 0;
}

int mycoroutines_switchto(co_t *co) {
    
    swapcontext(current_context, &co->context);

    return 0;
}

int mycoroutines_destroy(co_t *co) {
    
    free(co);

    return 0;
}
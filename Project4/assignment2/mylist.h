#ifndef __MY_LIST_H__
#define __MY_LIST_H__

#include "mythreads.h"

typedef struct node {
    int key;
    thr_t *thread;
    struct node *next;
    struct node *prev;
} node_t;

typedef struct list {
    node_t *head;
    node_t *active_process;
} list_t;

list_t *mylist_init();
void mylist_add(list_t *list, int key, thr_t *thr);
void mylist_remove(node_t *node);
void mylist_print(list_t *list);

#endif
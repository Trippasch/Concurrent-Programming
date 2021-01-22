#include "mylist.h"
#include <stdio.h>
#include <stdlib.h>

list_t *mylist_init() {
    list_t *list = (list_t *) malloc(sizeof(list_t));
    if (!list) {
        fprintf(stderr, "mylist_init: error in malloc()\n");
        exit(EXIT_FAILURE);
    }
    
    list->head = (node_t *) malloc(sizeof(node_t));
    if (!(list->head)) {
        free(list);
        exit(EXIT_FAILURE);
    }

    list->head->next = list->head;
    list->head->prev = list->head;
    list->active_process = NULL;

    return list;
}

void mylist_add(list_t *list, int key, thr_t *thr) {
    node_t *node = (node_t *) malloc(sizeof(node_t));
    if (!node) {
        fprintf(stderr, "mylist_add: error in malloc()");
        exit(EXIT_FAILURE);
    }

    if (list->head->next == list->head) {
        list->active_process = list->head;
    }
    
    node->key = key;
    node->thread = thr;
    node->next = list->head;
    node->prev = list->head->prev;

    list->head->prev->next = node;
    list->head->prev = node;
}

void mylist_print(list_t *list) {
    node_t *curr = list->head->next;

    while (curr != list->head) {
        printf("%d -> ", curr->key);
        curr = curr->next;
    }

    putchar('\n');
}

void mylist_remove(node_t *node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
}

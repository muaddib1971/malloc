#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include "malloc.h"

#define NUM_INTS 1<<16

#ifndef INTLIST
#define INTLIST
struct int_node{
        int data;
        struct int_node * next;
};

struct int_list{
        struct int_node * head;
        size_t num_nodes;
};

void intlist_init(struct int_list*);
bool intlist_add(struct int_list*, int);
void intlist_free(struct int_list*);
void intlist_fill(struct int_list*);
void intlist_print(struct int_list*);

#endif

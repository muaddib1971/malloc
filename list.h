#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef LIST_H
#define LIST_H

/**
 * the nodes that make up the linked list 
 **/
struct list_node
{
    struct list_node * next;
    struct list_node * prev;
    /* data stored as a void pointer so it can be handled generically */
    void * data;
};

/**
 * the list itself. Note the use of function pointers for the malloc and 
 * free implementations. This means I can write the same code and have 
 * it mean different things according to the specific allocator implementation.
 * That's the theory, anyway.
 **/
struct list
{
    /* pointers to head and tail as this is a doubly-linked list */
    struct list_node * head;
    struct list_node * tail;
    /* total nodes */
    size_t num_nodes;
    size_t size_data;
    /* function pointers to the various functions used specifically in this
     * library
     */
    void(*free)(void *);
    void*(*malloc)(size_t);
    int (*cmp)(const void*, const void*);
};

/* publicly callable functions in this module */
void list_init(struct list * list, void*(*malloc)(size_t), void(*free)(void*), 
    int (*cmp)(const void*, const void*), size_t);
bool list_add(struct list *, void *);
bool list_del(struct list*, void*);
void list_free(struct list *, struct list_node*);
void list_destroy(struct list*);

#endif

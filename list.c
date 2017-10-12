#include "list.h"

/**
 * initialise the linked list to empty and set the function pointers
 **/
void list_init(struct list * list, void*(*malloc)(size_t), void(*free)(void*), 
    int(*cmp)(const void*, const void*), size_t size_data)
{
    memset(list, 0, sizeof(struct list));
    list->malloc = malloc;
    list->free = free;
    list->cmp = cmp;
    list->size_data = size_data;
}

static struct list_node* new_node(struct list* list, void * data)
{
    struct list_node * new = list->malloc(sizeof(struct list_node));
    if(!new)
    {
        perror("malloc");
        return NULL;
    }
    new->data = list->malloc(list->size_data);
    if(!new->data)
    {
        perror("malloc");
        return NULL;
    }
    memcpy(new->data, data, list->size_data);
    new->prev = new->next = NULL;
    return new;
}

/**
 * add a node to the list. This involves duplicating the data. The nodes will
 * be inserted in sorted order according to the return value from 
 * list->cmp()
 **/
bool list_add(struct list * list, void * data)
{
    struct list_node * new = new_node(list, data);
    struct list_node * current = list->head;
    /* test if the list is empty */
    if(list->head == NULL)
    {
        list->head = list->tail = new;
        ++ list->num_nodes;
        return true;
    }
    /* iterate over the list to find the insertion point */
    while(current && list->cmp(current->data, data) < 0)
    {
        current = current->next;
    }
    /* not at the end of the list */
    if(current)
    {
        /* not at the beginning of the list */
        if(current->prev)
        {
            /* change the previous link */
            current->prev->next = new;
        }
        else
        {
            new->next = list->head;
            list->head->prev = new;
            list->head = new;
        }
        new->prev = current->prev;
        current->prev = new;
    }
    else
    {
        /* at the end of the list */
        list->tail->next = new;
        new->prev = list->tail;
        list->tail = new;
    }
    new->next = current;
    return true;
}

/**
 * Deletes the item specified from the list. Please note that in the list
 * header struct I have a separate del_cmp() function pointer as you might
 * want to provide a separate comparison for insertion compared to deletion.
 **/
bool list_del(struct list * list, void * data)
{

    struct list_node * current = list->head;
    if(!current)
    {
        return false;
    }
    while(current && list->cmp(current->data, data) < 0)
    {
        current = current->next;
    }
    if(!current)
    {
        return false;
    }
    if(!current->prev)
    {
        list->head = list->head->next;
        if(list->head)
        {
            list->head->prev = NULL;
        }
    }
    else
    {
        current->prev->next = current->next;
        if(current->next)
        {
            current->next->prev = current->prev;
        }
        else
        {
            list->tail = current->prev;
        }
    }
    list->free(current);
    -- list->num_nodes;
    return true;
}

void list_free(struct list * list, struct list_node* nodetofree)
{
    list->free(nodetofree->data);
    list->free(nodetofree);
}

void list_destroy(struct list * list)
{
    struct list_node * current = list->head;
    while( current )
    {
        struct list_node * prev = current;
        current = current->next;
        list_free(list, prev);
    }
}

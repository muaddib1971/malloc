#include "intlist.h"

void intlist_init(struct int_list* list){
    list->head = NULL;
    list->num_nodes = 0;
}

bool intlist_add(struct int_list* list, int data){
    struct int_node * new = simple_malloc(sizeof(struct int_node));
    struct int_node * current, *prev=NULL;
    if(!new)
    {
        fprintf(stderr, "Error: failed to allocate memory\n");
        return false;
    }
    new->next = NULL;
    new->data = data;
    if(list->num_nodes == 0){
        list->head = new;
        ++list->num_nodes;
        return true;
    }
    current = list->head;
    while(current && current->data < data){
        prev = current;
        current = current->next;
    }
    if(!prev){
        new->next = list->head;
        list->head = new;
    }
    else{
        assert(prev);
        new->next = current;
        prev->next = new;
    }
    ++ list->num_nodes;
    return true;
}

void intlist_free(struct int_list* list){
    struct int_node * current, * next;

    current = list->head;
    while(current){
        next = current;
        current = current->next;
        simple_free(next);
    }
}

void intlist_fill(struct int_list* list){
    srand(time(NULL));
    int count;
    for(count = 0; count < NUM_INTS; ++count){
        int random = rand();
        if(!intlist_add(list, random)){
            exit(EXIT_FAILURE);

        }
#ifdef DEBUG
        fprintf(stderr, "number of ints allocated: %d\n", 
                count + 1);
#endif
    }
}

void intlist_print(struct int_list* list){
    struct int_node * current;
    current = list->head;
    while(current){
        printf("%d\t", current->data);
        current = current->next;
    }
    putchar('\n');
}

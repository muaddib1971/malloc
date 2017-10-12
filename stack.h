#ifndef STACK_H
#define STACK_H

struct stack
{
    struct list stack_list;
    int size;
};

    
bool push(struct stack *, void * data);

void * pop(struct stack *);

void * peek(struct stack *);

#endif

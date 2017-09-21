#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "page.h"
#include "alloc.h"

#ifndef MALLOC_H
#define MALLOC_H
void malloc_init(size_t);
void* memory_alloc(size_t);
void* lazy_malloc(size_t);
void lazy_free(void*);
void * simple_malloc(size_t);
void simple_free(void*);

void memory_free(void*);
#endif

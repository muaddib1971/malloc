#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

enum os_result {
    SUCCESS, FAILURE
};

typedef enum{
    FALSE, TRUE
} BOOLEAN;

struct page{
    /* the bytes used on the page including header information */
    size_t bytes_used;
    struct page * next;
    struct page * prev;
    char data[];
};

struct alloc{
    void * address;
    size_t num_bytes;
    struct alloc * next;
    struct alloc * prev;
};

#define INVALID_BLOCK(B) ((B).address == NULL && (B).page == NULL)

void set_pagesize(size_t);
void* memory_alloc(size_t);
void memory_free(void*);

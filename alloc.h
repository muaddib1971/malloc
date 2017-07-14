#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#pragma pack(1)

#ifndef ALLOC_H
#define ALLOC_H
struct alloc{
        void * address;
        size_t num_bytes;
        struct alloc * next;
        struct alloc * prev;
};

struct block_pair{
        struct page * page;
        void * address;
};

struct page_list;

#define ALIGNED(N) 

#define INVALID_BLOCK(B) ((B).address == NULL && (B).page == NULL)
struct block_pair find_block(struct page_list * head_page, size_t size);
bool add_allocation(struct block_pair addr_data, size_t num_bytes,
                    size_t pagesize);

#endif
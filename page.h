#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#pragma pack(1)

#ifndef PAGE_H
#define PAGE_H
struct page_node;

struct page{
        /* the bytes used on the page including header information */
        size_t bytes_used;
        int page_no;
        struct page * prev;
        struct page * next;
        char data[];
};

struct page_list{
        struct page * head;
        size_t num_pages;
        size_t pagesize;
};

void pagelist_init(struct page_list *, size_t);
struct page * new_page(size_t size);
void add_page(struct page_list*, struct page * page);
void remove_page(struct page_list*, struct page* page);
struct page * get_page(struct page_list*, void*);
int max_pageno(struct page_list *);
void free_page(struct page_list *, struct page *);
#endif

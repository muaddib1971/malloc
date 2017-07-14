#include "page.h"
#include "malloc.h"
/*
  void pagelist_init(struct page_list *, size_t);
  void add_page(struct page_list*, struct page * page);
  void remove_page(struct page_list*, struct page* page);
  void free_page(struct page_list *, struct page *);
*/

int new_pageno(void){
        static int curnum = 0;
        return ++curnum;
}

void pagelist_init(struct page_list * list, size_t pagesize){
        memset(list, 0, sizeof(struct page_list));
        list->pagesize = pagesize;
}

void add_page(struct page_list * list, struct page * page){
        struct page * current;
        if(list->head == NULL){
                list->head = page;
                page->prev = page->next = NULL;
                ++ list->num_pages;
                return;
        }
        current = list->head;
        while(current->next && current->bytes_used < page->bytes_used){
                current = current->next;
        }
        if(!current->next){
                current->next = page;
                page->prev = current;
        }
        else{
                page->next = current->next;
                current->next = page;
                page->prev = current;
                if(page->next){
                        page->next->prev = page;
                }
        }
        ++ list->num_pages;
}

void remove_page(struct page_list * list, struct page* page){
        assert(page);
        if(page == list->head){
                list->head = list->head->next;
        }
        if(page->next){
                page->next->prev = page->prev;
        }
        if(page->prev){
                page->prev->next = page->next;
        }
        --list->num_pages;
}

struct page * get_page(struct page_list* pagelist, void * memory){
        struct page * current = pagelist->head;
        while(current){
                if((char*)memory >= current->data &&
                   (char*)memory < current->data + current->bytes_used){
                        return current;
                }
                current = current->next;
        }
        return NULL;
}

struct page * new_page(size_t size){
        struct page * newpage = sbrk(size);
        if(!newpage){
                return NULL;
        }
        memset(newpage, 0, size);
        newpage->page_no = new_pageno();
        return newpage;
}

void free_page(struct page_list * pagelist, struct page * page){
        remove_page(pagelist, page);
        if(page->page_no > max_pageno(pagelist)){
                brk(page);
        }
}

int max_pageno(struct page_list * pagelist){
        struct page * current;        
        int max = 0;
        current = pagelist->head;
        while(current){
                if(current->page_no > max){
                        max = current->page_no;
                }
                current = current->next;
        }
        return max;
}

#include "malloc.h"
static struct page * head_page = NULL;
static struct alloc * allocations = NULL;
static struct alloc * alloc_tail = NULL;
static size_t pagesize = 0;

struct block_pair{
    struct page * page;
    void * address;
};

struct block_pair find_block(size_t size){
    struct page * curr_page = head_page;
    struct block_pair result;
    memset(&result, 0, sizeof(struct block_pair));
    while(curr_page){
        if(pagesize - curr_page->bytes_used > size)
        {
            result.page = curr_page;
            result.address = curr_page->data+curr_page->bytes_used;
            return result;
        }
        curr_page = curr_page->next;
    }
    return result;
}

void set_pagesize(size_t page_size){
    pagesize = page_size;
}

void add_page(struct page * page){
    struct page * current;
    if(head_page == NULL){
        head_page = page;
        page->prev = page->next = NULL;
        return;
    }
    current = head_page;
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
}

void remove_page(struct page* page){
    assert(page);
    if(page == head_page){
        head_page = head_page->next;
    }
    if(page->next){
        page->next->prev = page->prev;
    }
    if(page->prev){
        page->prev->next = page->next;
    }
}

    static BOOLEAN 
add_allocation(struct block_pair addr_data, size_t num_bytes){

    struct alloc * new_alloc ;
    if(num_bytes  + sizeof(struct alloc) > 
            pagesize - sizeof(struct page) - addr_data.page->bytes_used){
        fprintf(stderr, "Error: allocation size is too large\n");
        return FALSE;
    }
    new_alloc = (struct alloc*)
        addr_data.page->data + addr_data.page->bytes_used;
    addr_data.page->bytes_used += num_bytes + sizeof(struct alloc);
    new_alloc->next = new_alloc->prev = NULL;
    new_alloc->address = ((char*)addr_data.address) + sizeof(struct alloc);
    new_alloc->num_bytes = num_bytes;
    addr_data.page->bytes_used += num_bytes;
    if(!allocations){
        allocations = alloc_tail = new_alloc;
    }
    else{
        alloc_tail->next = new_alloc;
        new_alloc->prev = alloc_tail;
        alloc_tail = new_alloc;
    }
    return TRUE;
}

void * memory_alloc(size_t size){
    struct block_pair alloc = find_block(size);
    if(INVALID_BLOCK(alloc)){
        struct page * newpage = sbrk(pagesize);
        if(!newpage){
            return NULL;
        }
        memset(newpage, 0, pagesize);
        newpage->next = head_page;
        head_page = newpage;
        alloc.address = newpage->data;
        alloc.page = newpage;
        newpage->bytes_used += size;
    }
    if(!add_allocation(alloc, size)){
        return NULL;
    }
    remove_page(alloc.page);
    add_page(alloc.page);
    return (char*)alloc.address+sizeof(struct alloc);
}

void memory_free(void * loc){
    /* we need to validate that this block was allocated by us and if so 
     * handle the reorganising of memory involved in our memory_freeing 
     * algorithm
     */
    struct page * page = get_page(loc);
    struct alloc * alloc;
    size_t alloc_bytes;
    size_t total_bytes;
    if(!page){
        fprintf(stderr, "Error: that memory was not allocated by us.\n");
        return;
    }
    alloc = (struct alloc*)(((char*)loc) - sizeof(struct alloc));
    alloc_bytes = alloc->num_bytes;
    brk(loc);

}


int main(void){
    char * newstr;
    set_pagesize(1024);
    newstr = memory_alloc(strlen("hello")+1);
    if(!newstr){
        fprintf(stderr, "Error: failure to allocate memory.\n");
        return FAILURE;
    }
    strcpy(newstr, "hello");
    printf("%s\n", newstr);
    memory_free(newstr);
    return SUCCESS;
}

#include "malloc.h"
#include "page.h"

static struct page_list page_list;

void * memory_alloc(size_t size)
{
    struct block_pair alloc = find_block(&page_list, size);
    if(INVALID_BLOCK(alloc)){
        struct page * newpage = new_page(page_list.pagesize);
        if(!newpage){
            return NULL;
        }
        add_page(&page_list, newpage);
        alloc.address = newpage->data;
        alloc.page = newpage;
    }
    if(!add_allocation(alloc, size, page_list.pagesize)){
        return NULL;
    }
    remove_page(&page_list, alloc.page);
    add_page(&page_list, alloc.page);
    return ((char*)alloc.address)+sizeof(struct alloc);
}

void * simple_malloc(size_t size)
{
    void * chunk = simple_alloc(size);
    if(!chunk)
    {
        perror("failed to allocate memory");
        return NULL;
    }
    return chunk;
}


void * lazy_malloc(size_t size)
{
    void * current = sbrk(0);
    void * new = sbrk(size);
    if(new == (void*) -1)
    {
        perror("sbreak failed");
        return NULL;
    }
    assert(current == new);
    return new;
}

void lazy_free(void * mem)
{
    (void) mem;
}

void simple_free(void * mem)
{
    simple_dealloc(mem);
}


void memory_free(void * loc){
    /* we need to validate that this block was allocated by us and if so 
     * handle the reorganising of memory involved in our memory_freeing 
     * algorithm
     */
    struct page * page = get_page(&page_list, loc);
    struct alloc * alloc;
    size_t alloc_bytes;
    size_t total_bytes;

    if(!page)
    {
        fprintf(stderr, "Error: memory address %p was not "
                "allocated by us.\n", loc);
        return;
    }
    alloc = (struct alloc*)(((char*)loc) - sizeof(struct alloc));
    alloc_bytes = alloc->num_bytes;
    total_bytes = page->bytes_used;
    memmove(alloc, ((char*)alloc) + alloc_bytes,
            total_bytes - alloc_bytes);
    page->bytes_used -= alloc_bytes;
    if(page->bytes_used == 0)
    {
        free_page(&page_list, page);
    }
}

void malloc_init(size_t pagesize)
{
    pagelist_init(&page_list, pagesize);
}

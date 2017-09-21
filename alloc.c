#include "alloc.h"
#include "page.h"

static struct alloc * allocations = NULL;
static struct alloc * alloc_tail = NULL;

static struct simple_alloc * simple_allocs_head = NULL;
static struct simple_alloc * simple_allocs_tail = NULL;

static struct simple_alloc * simple_frees_head = NULL;



struct block_pair find_block(struct page_list * pagelist, size_t size)
{
    struct page * curr_page = pagelist->head;
    struct block_pair result;
#ifdef DEBUG
    fprintf(stderr, "Size to look for: %ld\n", size);
#endif
    memset(&result, 0, sizeof(struct block_pair));
    while(curr_page){
        if(pagelist->pagesize - sizeof(struct page) -
                curr_page->bytes_used > size + sizeof(struct alloc))
        {
            result.page = curr_page;
            result.address = 
                curr_page->data+curr_page->bytes_used;
            return result;
        }
        curr_page = curr_page->next;
    }
    /* if we get here there is no page with a big enough space for
       the allocation
       */
    return result;
}

bool add_allocation(struct block_pair addr_data, size_t num_bytes,
        size_t pagesize)
{
    struct alloc * new_alloc ;
    size_t total_bytes = num_bytes + sizeof(struct alloc);
    size_t available = pagesize - sizeof(struct page)
        - addr_data.page->bytes_used;
    if(total_bytes > available){
        fprintf(stderr, "Error: allocation size is too large\n");
        fprintf(stderr, "Bytes wanted: %ld\t Bytes available: %ld\n", 
                total_bytes, available);
        return false;
    }
    new_alloc = (struct alloc*)
        addr_data.page->data + addr_data.page->bytes_used;
    addr_data.page->bytes_used += total_bytes;
    new_alloc->next = new_alloc->prev = NULL;
    new_alloc->address = ((char*)addr_data.address) 
        + sizeof(struct alloc);
    new_alloc->num_bytes = total_bytes;
    if(!allocations){
        allocations = alloc_tail = new_alloc;
    } else {
        alloc_tail->next = new_alloc;
        new_alloc->prev = alloc_tail;
        alloc_tail = new_alloc;
    }
    return true;
}

static void simple_alloc_append(struct simple_alloc* alloc)
{
    if(!simple_allocs_head)
    {
        simple_allocs_head = simple_allocs_tail = alloc;
        alloc->prev = alloc->next = NULL;
        return;

    }
    simple_allocs_tail->next = alloc;
    simple_allocs_tail = alloc;

}

struct simple_alloc * find_block_simple(size_t size)
{
    return NULL;
    
}



void * simple_alloc(size_t size)
{
    struct simple_alloc * alloc;
    
    
    if(alloc = find_block_simple(size), !alloc)
    {
	void * current = sbrk(0);
	alloc = sbrk(size + sizeof(struct simple_alloc));  
	if(alloc == (void*)-1)
	{
	    perror("alloc failed");
	    return NULL;	
	}
	assert(current == alloc);
	alloc->address = alloc + 1;
	alloc->num_bytes = size;
    }
    alloc->prev = alloc->next = NULL;
    simple_alloc_append(alloc);
    return alloc->address;
}

static void simple_frees_add(struct simple_alloc * alloc)
{
    struct simple_alloc * current = simple_frees_head;
    if(!simple_frees_head)
    {
	simple_frees_head = alloc;
	alloc->next = NULL;
	return;
	
    }
    
    while(current && current->num_bytes < alloc->num_bytes)
    {
        if(current->next)
        {	    
            current = current->next;
        } else {
            break;
        }
    }
    alloc->next = current->next;
    current->next = alloc;
}

static void simple_allocs_remove(struct simple_alloc * node)
{
    struct simple_alloc * current = simple_allocs_head;
    struct simple_alloc * prev = NULL;

    while(current)
    {

        if(current == node)
        {
            if(prev)
            {
                prev->next = current->next;
            } else {
                simple_allocs_head = simple_allocs_head->next;		
            }
            return;	    
        }
        prev = current;
        current = current->next;	
    }
    current->next = NULL;
    
}


void simple_dealloc(void* ptr)
{
    struct simple_alloc * node
        = (struct simple_alloc*)(((char*)ptr) - sizeof(struct simple_alloc));
#if DEBUG
    fprintf(stderr, "simple free called with memory address %p\n", ptr);
#endif
    simple_allocs_remove(node);    
    simple_frees_add(node);
}

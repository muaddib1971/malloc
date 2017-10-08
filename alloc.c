#include "alloc.h"
#include "page.h"

/* objects which are private to this module - the cannot be accessed 
 * outside of here
 */
static struct alloc * allocations = NULL;
static struct alloc * alloc_tail = NULL;

/* the list for all the allocations made with the simple_malloc algorithm */
static struct simple_alloc * simple_allocs_head = NULL;

/* the tail for that same list so we can do fast insertion - each new
 * allocation is simply added to the end of the list 
 */
static struct simple_alloc * simple_allocs_tail = NULL;
/* the head of the list of frees. Please note that as these are inserted 
 * in order of the size of the chunk, we don't store the tail as there's 
 * no point. We always have to iterate the list. This makes freeing of 
 * memory much slower than allocation.
 */
static struct simple_alloc * simple_frees_head = NULL;
static struct simple_alloc * simple_frees_tail = NULL;

static void * frontier = NULL;

struct block_pair find_block(struct page_list * pagelist, size_t size)
{
    struct page * curr_page = pagelist->head;
    struct block_pair result;
#ifdef DEBUG
    fprintf(stderr, "Size to look for: %ld\n", size);
#endif
    memset(&result, 0, sizeof(struct block_pair));
    while(curr_page)
    {
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
    }
    else
    {
        alloc->next = NULL;
        alloc->prev = simple_allocs_tail;
        simple_allocs_tail->next = alloc;
        simple_allocs_tail = alloc;
    }
}

static void simple_allocs_remove(struct simple_alloc * node)
{
    /* greb the head of the list */
    struct simple_alloc * current = simple_allocs_head;

    while(current)
    {
        if(current == node)
        {
            if(current->prev)
            {
                current->prev->next = current->next;
            } 
            else 
            {
                simple_allocs_head = simple_allocs_head->next;		
            }
            if(current->next)
            {
                current->next->prev = current->prev;
            }
            else
            {
                simple_allocs_tail = simple_allocs_tail->prev;
            }
            current->prev = current->next = NULL;
            return;	    
        }
        current = current->next;	
    }
}

static void simple_merge(struct simple_alloc * prev,
        struct simple_alloc * next)
{
    assert((struct simple_alloc*)(prev->data +
                prev->num_bytes) == next);
#ifdef DEBUG
    fprintf(stderr, "lets merge %p and %p\n", (void*)prev, (void*)next);
#endif
    simple_allocs_remove(next);
    prev->num_bytes += next->num_bytes + sizeof(struct simple_alloc);
}

static bool simple_frees_remove(struct simple_alloc * node)
{
    struct simple_alloc *current;
    current = simple_frees_head;

    while(current && current != node)
    {
        current = current->next;
    }
    if(!current)
    {
        return false;
    }
    if(current->prev)
    {
        current->prev->next = current->next;
    } 
    else 
    {
        simple_allocs_head = current->next;
    }
    if(current->next)
    {
        current->next->prev = current->prev;
    }
    else
    {
        simple_allocs_tail = current->prev;
    }
    current->next = current->prev = NULL;
    return true;
}

struct simple_pair
{
    struct simple_alloc * first;
    struct simple_alloc * second;    
};

static struct simple_pair simple_split(struct simple_alloc * node,
        size_t num_bytes)
{
    struct simple_pair split_pair;
#ifdef DEBUG
    fprintf(stderr, "let's split %p\n", (void*)node);
#endif
    memset(&split_pair, 0, sizeof(struct simple_pair));    
    assert(node->num_bytes >= num_bytes + 2 * sizeof(struct simple_alloc));
    simple_frees_remove(node);
    split_pair.first = node;
    node->num_bytes -= num_bytes;
    split_pair.second = 
        (struct simple_alloc*)(node->data + node->num_bytes);
    return split_pair;
}

static void simple_frees_add(struct simple_alloc * alloc)
{
    struct simple_alloc * current = simple_frees_head;
    if(!simple_frees_head)
    {
        simple_frees_head = alloc;
        simple_frees_tail = alloc;
        return;
    }
    while(current && current < alloc)
    {
        current = current->next;
    }

    alloc->next = current;
    if(current)
    {
        alloc->prev = current->prev;
        current->prev = alloc;
        if(current->prev)
        {
            current->prev->next = alloc;
        }
        else
        {
            simple_frees_head = alloc;
        }
    }
    else
    {
        alloc->prev = simple_frees_tail;
        simple_frees_tail->next = alloc;
        simple_frees_tail = alloc;
    }

    if(alloc->prev
            && ((struct simple_alloc*)(alloc->prev->data) 
                + alloc->prev->num_bytes)
            == current)
    {
        simple_frees_remove(alloc->prev);
        alloc = alloc->prev;
        simple_merge(alloc, alloc->next);
    }

    if((struct simple_alloc*)(alloc->data + 
                alloc->num_bytes) == current)
    {
        simple_frees_remove(current);
        simple_merge(alloc, current);
    }
}

struct simple_alloc * find_block_simple(size_t size)
{
    struct simple_alloc * current = simple_frees_head;
    struct simple_alloc * prev = NULL;

    while(current && current->num_bytes < size)
    {
        current = current->next;	
    }
    if(current)
    {
        assert(current->num_bytes >= size);
        if(current->num_bytes >= size + 2 * sizeof(struct simple_alloc))
        {
            struct simple_pair pair;
            pair = simple_split(current, size);
            simple_alloc_append(pair.first);
            simple_frees_add(pair.second);
            return pair.first;
        }

        if(prev)
        {
            prev->next = current->next;
        } 
        else 
        {
            simple_frees_head = current->next;	    
        }
    }
    if(current)
    {
        current->next = current->prev = NULL;
    }
    return current;
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
        alloc->num_bytes = size;
    }
    alloc->prev = alloc->next = NULL;
    simple_alloc_append(alloc);
    frontier = alloc;
    return alloc->data;
}

void simple_dealloc(void* ptr)
{
    struct simple_alloc * node
        = (struct simple_alloc*)(((char*)ptr) 
                - sizeof(struct simple_alloc));
#if DEBUG
    fprintf(stderr, "simple free called with memory address %p\n", ptr);
#endif
    simple_allocs_remove(node);    
    simple_frees_add(node);
}


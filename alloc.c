#include "alloc.h"
#include "page.h"

static struct alloc * allocations = NULL;
static struct alloc * alloc_tail = NULL;

struct block_pair find_block(struct page_list * pagelist, size_t size)
{
        struct page * curr_page = pagelist->head;
        struct block_pair result;
        memset(&result, 0, sizeof(struct block_pair));
        while(curr_page){
                if(pagelist->pagesize - sizeof(struct page) -
                   curr_page->bytes_used > size)
                {
                        result.page = curr_page;
                        result.address = curr_page->data+curr_page->bytes_used;
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
                return false;
        }
        new_alloc = (struct alloc*)
                addr_data.page->data + addr_data.page->bytes_used;
        addr_data.page->bytes_used += total_bytes;
        new_alloc->next = new_alloc->prev = NULL;
        new_alloc->address = ((char*)addr_data.address) + sizeof(struct alloc);
        new_alloc->num_bytes = total_bytes;
        if(!allocations){
                allocations = alloc_tail = new_alloc;
        }
        else{
                alloc_tail->next = new_alloc;
                new_alloc->prev = alloc_tail;
                alloc_tail = new_alloc;
        }
        return true;
}

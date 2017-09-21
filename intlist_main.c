#include "intlist.h"

int main(void){
        struct int_list list;
#if 0
        malloc_init(1024);
#endif
        intlist_init(&list);
        intlist_fill(&list);
	intlist_print(&list);
	
        intlist_free(&list);
        return EXIT_SUCCESS;
}

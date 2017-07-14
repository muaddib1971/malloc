#include "intlist.h"

int main(void){
        struct int_list list;
        malloc_init(1024);
        intlist_init(&list);
        intlist_fill(&list);
        intlist_free(&list);
        return EXIT_SUCCESS;
}

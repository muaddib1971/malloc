#include "malloc.h"
#include <stdlib.h>
#include <stdio.h>

int main(void){
        char * newstr;
        malloc_init(1024);
        newstr = memory_alloc(strlen("hello")+1);
        if(!newstr){
                fprintf(stderr, "Error: failure to allocate memory.\n");
                return EXIT_FAILURE;
        }
        strcpy(newstr, "hello");
        printf("%s\n", newstr);
        memory_free(newstr);
        return EXIT_SUCCESS;
}

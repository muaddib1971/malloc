SOURCES=alloc.c malloc.c page.c intlist.c
OBJECTS=alloc.o malloc.o page.o intlist.o
HEADERS=alloc.h malloc.h page.h intlist.h
CC=gcc
CFLAGS=-Wall -pedantic -Wextra -Werror -fdiagnostics-color=always -g
LDFLAGS=

all: tags strmalloc intlist

intlist: $(OBJECTS) intlist_main.o
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) intlist_main.o

strmalloc:$(OBJECTS) strmalloc.o
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) strmalloc.o 

%.o:%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

tags:
	ctags $(SOURCES) $(HEADERS)

sanitize:CFLAGS+=-fsanitize=address
sanitize:LDFLAGS+=-fsanitize=address
sanitize:clean all

clean:
	rm -f *.o *~ malloc_test \#* tags

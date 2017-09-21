SOURCES=alloc.c malloc.c page.c intlist.c
OBJECTS=alloc.o malloc.o page.o intlist.o
HEADERS=alloc.h malloc.h page.h intlist.h
CC=gcc
CFLAGS=-Wall -pedantic -Wextra -Werror -g -fdiagnostics-color=always
LDFLAGS=

all: tags strmalloc intlist

intlist: $(OBJECTS) intlist_main.o
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) intlist_main.o

strmalloc:$(OBJECTS) strmalloc.o
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) strmalloc.o 

%.o:%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

debug:CFLAGS+=-g -DDEBUG
debug:clean all

tags:
	ctags $(SOURCES) $(HEADERS)

sanitize:CFLAGS+=-fsanitize=address -g -DDEBUG
sanitize:LDFLAGS+=-fsanitize=address
sanitize:clean all

clean:
	rm -f *.o *~ strmalloc intlist \#* tags

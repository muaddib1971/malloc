SOURCES=alloc.c malloc.c page.c intlist.c intlist_main.c
OBJECTS=alloc.o malloc.o page.o intlist.o intlist_main.o
HEADERS=alloc.h malloc.h page.h intlist.h
CC=gcc
CFLAGS=-Wall -pedantic -Wextra -Werror -g -fdiagnostics-color=always
LDFLAGS=

all: tags intlist_lazy intlist_simple

intlist_lazy:CFLAGS=-Wall -pedantic -Wextra -Werror -g \
	-fdiagnostics-color=always -DLAZY_ALLOC
intlist_lazy: clean_objects $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

intlist_simple:CFLAGS=-Wall -pedantic -Wextra -Werror -g \
	-fdiagnostics-color=always -DSIMPLE_ALLOC
intlist_simple: clean_objects $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o:%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

debug:CFLAGS+=-g -DDEBUG
debug:clean all

tags:
	ctags $(SOURCES) $(HEADERS)

sanitize:CFLAGS+=-fsanitize=address -g -DDEBUG
sanitize:LDFLAGS+=-fsanitize=address
sanitize:clean all

.PHONY:clean_objects
clean_objects:
	rm -f *.o


.PHONY:clean
clean: clean_objects
	rm -f  *~  \#* tags intlist_lazy intlist_simple

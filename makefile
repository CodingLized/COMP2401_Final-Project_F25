OBJECT_FILES = main.o ghosts.o hunters.o helpers.o
HEADER_FILES = defs.h helpers.h

all: main ghosts hunters helpers
	gcc $(OBJECT_FILES) -o finalProject
main: main.c ghosts hunters helpers
	gcc -c main.c
ghosts: ghosts.c helpers defs.h
	gcc -c ghosts.c
hunters: hunters.c helpers defs.h
	gcc -c hunters.c
helpers: helpers.c helpers.h
	gcc -c helpers.c
clean:
	rm *.o *~ 

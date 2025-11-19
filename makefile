OBJECT_FILES = main.o ghosts.o hunters.o house.o helpers.o 
HEADER_FILES = defs.h helpers.h


all: main ghosts hunters house helpers
	gcc $(OBJECT_FILES) -o finalProject
all-debug: main-debug ghosts-debug hunters-debug house-debug helpers-debug
	gcc $(OBJECT_FILES) -o finalProject

main: main.c $(HEADER_FILES)
	gcc -c main.c
main-debug: main.c $(HEADER_FILES)
	gcc -g -c main.c	

ghosts: ghosts.c $(HEADER_FILES)
	gcc -c ghosts.c
ghosts-debug: ghosts.c $(HEADER_FILES)
	gcc -g -c ghosts.c	
hunters: hunters.c $(HEADER_FILES)
	gcc -c hunters.c
hunters-debug: hunters.c $(HEADER_FILES)
	gcc -g -c hunters.c	

house: house.c $(HEADER_FILES)
	gcc -c house.c
house-debug: house.c $(HEADER_FILES)
	gcc -g -c house.c	

helpers: helpers.c $(HEADER_FILES)
	gcc -c helpers.c 
helpers-debug: helpers.c $(HEADER_FILES)
	gcc -g -c helpers.c 	
	
clean:
	rm -f $(OBJECT_FILES) finalProject

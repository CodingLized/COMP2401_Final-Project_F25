OBJECT_FILES = main.o ghosts.o hunters.o house.o helpers.o 
HEADER_FILES = defs.h helpers.h
W = -Wall

all: $(OBJECT_FILES)
	gcc -Wall $(OBJECT_FILES) -o finalProject
all-debug: main-debug ghosts-debug hunters-debug house-debug helpers-debug
	gcc -Wall $(OBJECT_FILES) -o finalProject

main.o: main.c $(HEADER_FILES)
	gcc -Wall -c main.c
main-debug: main.c $(HEADER_FILES)
	gcc -Wall -g -c main.c	

ghosts.o: ghosts.c $(HEADER_FILES)
	gcc -Wall -c ghosts.c
ghosts-debug: ghosts.c $(HEADER_FILES)
	gcc -Wall -g -c ghosts.c	
hunters.o: hunters.c $(HEADER_FILES)
	gcc -Wall -c hunters.c
hunters-debug: hunters.c $(HEADER_FILES)
	gcc -Wall -g -c hunters.c	

house.o: house.c $(HEADER_FILES)
	gcc -Wall -c house.c
house-debug: house.c $(HEADER_FILES)
	gcc -Wall -g -c house.c	

helpers.o: helpers.c $(HEADER_FILES)
	gcc -Wall -c helpers.c 
helpers-debug: helpers.c $(HEADER_FILES)
	gcc -Wall -g -c helpers.c 	

clean:
	rm -f $(OBJECT_FILES) finalProject

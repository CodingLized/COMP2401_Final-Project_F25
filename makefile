SOURCE_FILES = main.c ghosts.c house.c hunters.c helpers.c 
OBJECT_FILES = main.o ghosts.o hunters.o house.o helpers.o
HEADER_FILES = defs.h helpers.h
COMPILE_OPTS = -Wall -g -c

all: $(OBJECT_FILES)
	gcc -Wall $(OBJECT_FILES) -o finalProject

main.o: main.c $(HEADER_FILES)
	gcc $(COMPILE_OPTS) main.c

ghosts.o: ghosts.c $(HEADER_FILES)
	gcc $(COMPILE_OPTS) ghosts.c

hunters.o: hunters.c $(HEADER_FILES)
	gcc $(COMPILE_OPTS) hunters.c

house.o: house.c $(HEADER_FILES)
	gcc $(COMPILE_OPTS) house.c

helpers.o: helpers.c $(HEADER_FILES)
	gcc $(COMPILE_OPTS) helpers.c 

clean:
	rm -f $(OBJECT_FILES)

#Additional labels used in development

clean-full:
	rm -f *.csv $(OBJECT_FILES) finalProject 	
	
archive:
	tar -cvf final_project.tar $(SOURCE_FILES) $(HEADER_FILES) makefile README.md validate_logs.py
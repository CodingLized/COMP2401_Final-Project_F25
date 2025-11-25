#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "defs.h"
#include "helpers.h"

//#define RUN_THREADS 1
//#define TEST
static void get_hunter_info(Hunter* h);
static void run_single_thread(House* house, Ghost* ghost);

int main() {

    /*
    1. Initialize a House structure.
    2. Populate the House with rooms using the provided helper function.
    3. Initialize all of the ghost data and hunters.
    4. Create threads for the ghost and each hunter.
    5. Wait for all threads to complete.
    6. Print final results to the console:
         - Type of ghost encountered.
         - The reason that each hunter exited
         - The evidence collected by each hunter and which ghost is represented by that evidence.
    7. Clean up all dynamically allocated resources and call sem_destroy() on all semaphores.
    */

    /*
    TODO
    *Remember to include logging functions

    *According to prof, only return to exit when you have evidence
    If emotions are too high, simply remove pointer from room, store exit reason and flag as done with sim

    Main Flow[v]
    
    Setup[]
    -room_init[v]
    -rooms_connect[v] 
    -ghost_init[v]
    -hunter_collection_init[]
    
    run_single_thread[]
        PLAN
        -LOOP until ?
            -Go through each hunter loop
            -Go through ghost loop





    ghost_thread[]
        -Loop until ghost too bored

    hunter_threads[]

    Display results[]
    
    */

    /*
    ----ISSUES----
    
    
    */

    House house; 
    Ghost ghost;
    char hunter_name[MAX_HUNTER_NAME];
    int hunter_id; 
    
    
    //Set up
    house_populate_rooms(&house);
    ghost_init(&ghost, &house); 
    #ifndef TEST
    HunterCollection* hc = &house.hunterCollection;
    hunter_collection_init(&hc);


    //Hunter info input loop
    while(true){  
        printf("Enter hunter name: ");
        if(fgets(hunter_name, MAX_HUNTER_NAME, stdin) == NULL){
            printf("\nInput cannot be blank.\n");
        }
        else if (strncmp(hunter_name, "done\n", 5)==0){
           break;
        }
        else{
            for(int i = 0; i < MAX_HUNTER_NAME; i++){ //Replaces newline character read in with null char
                if(hunter_name[i]=='\n'){
                    hunter_name[i] = 0;
                }
            }

            printf("Enter hunter ID: ");
            scanf("%d", &hunter_id);
            while(getchar()!='\n');//Clear input buffer

            hunter_init(&house, hunter_name, hunter_id);
        }
        

    }

    #ifdef RUN_THREADS
    //Create ghost thread

    //Create hunter threads
    
    #endif


    #ifndef RUN_THREADS

    //run_single_thread(&house, &ghost);

    #endif

    

    //display_results(&house);

    hunter_collection_cleanup(&house.hunterCollection);
#endif
#ifdef TEST
    for(int i = 0; i < house.room_count; i++){
        printf("%s initialized\n", house.rooms[i].name);
    }
#endif    
    return 0;

}

static void get_hunter_info(Hunter* h){}

static void run_single_thread(House* house, Ghost* ghost){


}
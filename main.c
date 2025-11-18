#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "defs.h"
#include "helpers.h"

//#define RUN_THREADS 1

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


    House house; 
    Ghost ghost;
    bool user_done = false; 
    char hunter_name[MAX_HUNTER_NAME];
    int hunter_id; 
    
    //Set up
    house_populate_rooms(&house);
    ghost_init(&ghost); 
    hunter_collection_init(&house.hunters);


    //Hunter info input loop
    while(true){  
        printf("Enter hunter name: ");
        
        if(fgets(hunter_name, MAX_HUNTER_NAME, stdin) == NULL){
            printf("\nInput cannot be blank.\n");
        }
        else if (strncmp(hunter_name, "done", 5)==0){//ISSUE: DOES NOT QUIT LOOP WHEN DONE IS TYPED
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
            hunter_add(&house, hunter_name, hunter_id);
            printf("Hunter added.\n");

        }
        

    }


    run_single_thread(&house, &ghost);

    #ifdef RUN_THREADS
    //Create ghost thread

    //Create hunter threads
    
    #endif


    return 0;
}

static void get_hunter_info(Hunter* h){}

static void run_single_thread(House* house, Ghost* ghost){

}

#include "defs.h"
#include "helpers.h"
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
/**
 * @brief Initializes a dynamic array of hunters
 * @param[out] hc The dynamic hunter array to be initialized
 */
void hunter_collection_init(HunterCollection* *hc){
    (*hc)->hunters = calloc(1, sizeof(Hunter*));
    (*hc)->hunters[0] = NULL;

    (*hc)->size = 0;

}

/**
 * @brief Finds the index of the pointer to a hunter in the hunters current room
 */
int get_hunter_index(Hunter* hunter, Room* room){
    return 0;
}

/**
 * @brief Intializes a hunter and adds them to a random room in the house
 * @param[out] house The house the hunter will be added to
 * @param[in] name The name of the hunter
 * @param[in] id The hunter ID
 */
void hunter_init(House* house, const char* name, int id){
    Hunter* new_hunter = malloc(sizeof(Hunter));

    strncpy(new_hunter->name, name, MAX_HUNTER_NAME);
    new_hunter->id = id;
    new_hunter->curr_room = &house->rooms[0]; //Add hunter to the starting van
    new_hunter->case_file = &house->case_file;

    //Give hunter a random device
    select_rand_device(new_hunter);

    new_hunter->fear = 0;
    new_hunter->boredom = 0;
    new_hunter->return_to_exit = false;
    new_hunter->hasExited = false;

    hunter_trail_init(new_hunter);

    //Add hunter to house
    hunter_collection_add(new_hunter, house);

    Hunter* *hunters = house->hunterCollection.hunters; //For convenience
    log_hunter_init(hunters[house->hunterCollection.size-1]->id, hunters[house->hunterCollection.size-1]->curr_room->name, hunters[house->hunterCollection.size-1]->name, hunters[house->hunterCollection.size-1]->device_type);
}

/**
 * @brief Adds a hunter to the HunterCollection
 * @param[in] hunter The hunter to be added
 * @param[in,out] house The house that contains the HunterCollection
 */
void hunter_collection_add(Hunter* hunter, House* house){
    HunterCollection* h_collection = &house->hunterCollection;

    //Increase hunterCollection capacity
    Hunter* *temp = h_collection->hunters;
    h_collection->hunters = calloc(h_collection->size+1, sizeof(Hunter*));
    if(h_collection->size > 0){
        for(int i = 0; i < h_collection->size; i++){
            h_collection->hunters[i] = temp[i];
        }
    }

    h_collection->hunters[h_collection->size++] = hunter;
    free(temp);
}

/**
 * @brief Selects a random device for a hunter
 * @param[out] device The pointer that will capture the random device
 */
void select_rand_device(Hunter* hunter){
    const EvidenceType *type_list;
    int type_list_size = get_all_evidence_types(&type_list);

    int device_index = rand_int_threadsafe(0, type_list_size);
    if(type_list[device_index] == hunter->device_type){//Ensures the same device cannot be selected twice
        device_index = (device_index + 1) % type_list_size;
    }
    hunter->device_type = type_list[device_index];
}

/**
 * @brief Initializes a hunter's breadcrumb stack
 * @param[out] hunter A Hunter structure
 */
void hunter_trail_init(Hunter* hunter){
    hunter->trail.top = NULL;
}

/**
 * @brief Adds the current room to the hunter's breadcrumb stack
 * @param[in,out] hunter A Hunter structure
 */
void hunter_trail_push(Hunter* hunter){
    RoomNode* previous = hunter->trail.top;

    RoomNode* new_node = malloc(sizeof(RoomNode));
    new_node->data = hunter->curr_room;
    hunter->trail.top = new_node;
    new_node->previous = previous;

}

/**
 * @brief Removes the room at the top from the hunter's breadcrumb stack and returns it
 * @param[in,out] hunter A Hunter structure
 * @param[out] room The address to store the popped room
 */
void hunter_trail_pop(Hunter* hunter, Room* *room){
    RoomNode* popped = hunter->trail.top;
    printf("Hunter (%d) popping a room\n", hunter->id);
    if(popped == NULL){
        *room = NULL;
        return;
    }

    *room = popped->data;
    hunter->trail.top = popped->previous;
    printf("Popped room = %s\n", popped->data->name);
    free(popped);


}

/**
 * @brief Clears all nodes from the hunter's breadcrumb stack
 * @param[in,out] hunter A Hunter structure
 */
void hunter_trail_clear(Hunter* hunter){
    Room* throw_away;

    while(throw_away!=NULL){
        hunter_trail_pop(hunter, &throw_away);
    }
}

/**
 * @brief Goes through one loop of actions for a hunter's turn
 * @param[in] hunter the Hunter structure that will take action
 */
void hunter_take_action(Hunter* hunter){
    //Rmr to check if hunter is still in sim
    
    if(hunter_check_ghost(hunter)){
        printf("Hunter notices ghost\n");
        hunter->boredom = 0;
        hunter->fear++;

    }
    else{
        hunter->boredom++;
    }
    if(hunter_in_exit(hunter)){
        hunter->return_to_exit = false;
        hunter_trail_clear(hunter);

        if(hunter_check_victory(hunter)){
            hunter->exit_reason = LR_EVIDENCE;
            hunter_exit_simulation(hunter);
        }
        else{
            select_rand_device(hunter);
        }

        
    }
    else{//Review this branch
        if(hunter_check_emotions(hunter)){
            hunter_exit_simulation(hunter);
        }
        else{
            //int evidence_pos = -1;
            int evidence_pos = hunter_check_evidence(hunter); 
            if(evidence_pos > -1){
                printf("Evidence found by Hunter (%d)\n", hunter->id);
                clear_bit(&hunter->curr_room->evidence, evidence_pos); //Lock mutex before setting
                set_bit(&hunter->case_file->collected, evidence_pos); 
                printf("CASEFILE: %d\n", hunter->case_file->collected);
                hunter->return_to_exit = true;
            }
            else{
                //A 5% chance that the hunter will return to the exit
                int chance = rand_int_threadsafe(1, 21);
                if(chance == 1){
                    hunter->return_to_exit = true;
                }
            }
        }
    }
    if(hunter->return_to_exit && !hunter->hasExited){
        Room *popped_room;
        hunter_trail_pop(hunter, &popped_room);
        if(popped_room != NULL){
            hunter_move(hunter, popped_room); 
        }
        
    }
    else if(!hunter->hasExited){
        //Select a random connected room
        Room* target_room;
        int room_index;
        do{
            room_index = rand_int_threadsafe(0, hunter->curr_room->connections_no);
            target_room = hunter->curr_room->connected_rooms[room_index];
            printf("Target room address: %ld\n", (long int)target_room);
        }
        while(target_room == NULL);
        

        
        hunter_move(hunter, target_room); 
    }
    printf("Hunter %s [Fear=%d, Boredom=%d, Room=%s]\n", hunter->name, hunter->fear, hunter->boredom, hunter->curr_room->name);   
}

/**
 * @brief Determines whether a ghost is present in the room with the hunter
 * @param[in] hunter A pointer to the hunter of interest
 * @return true or false depending on whether a ghost is present
 */
bool hunter_check_ghost(Hunter* hunter){
    return hunter->curr_room->ghost != NULL;
}

bool hunter_in_exit(Hunter* hunter){
    return hunter->curr_room->isExit;
}
void hunter_check_exited(Hunter* hunter){

}
bool hunter_check_victory(Hunter* hunter){
    //
    
    
    
    
    return false;
}

/**
 * @brief Determines hunter's fear or boredom has reached its max
 * @param[in] hunter A pointer to the hunter of interest
 * @return true or false depending on whether max emotions were reached
 */
bool hunter_check_emotions(Hunter* hunter){
    if(hunter->boredom >= ENTITY_BOREDOM_MAX){
        hunter->exit_reason = LR_BORED;
        return true;
    }
    else if(hunter->fear >= HUNTER_FEAR_MAX){
        hunter->exit_reason = LR_AFRAID;
        return true;
    }
    return false;
}

/**
 * @brief Checks to see if the hunter device type matches any evidence bit in the room
 * @param[in] hunter The hunter checking the room evidence
 * @return An integer greater than zero of there is a match
 */
int hunter_check_evidence(Hunter* hunter){
    EvidenceByte device_to_evidence = (EvidenceByte)hunter->device_type;
    int device_bit_index;

    for(int i = 0; i < 8; i++){//Get the index of the bit that is set for the device
        if(get_bit(&device_to_evidence, i)){
            device_bit_index = i;
            break;
        }
    }

    if(get_bit(&hunter->curr_room->evidence, device_bit_index)){//Compare device with room evidence
        return device_bit_index;
    } 
    printf("Hunter (%d): No evidence found\n", hunter->id);
    return -1; //There isn't a match
    
}
void hunter_move(Hunter* hunter, Room* target_room){
    //--Define in move
        //If target room is full, return (move failed)
        //Otherwise
        //      first_mutex = Compare names of target room and curr_room to determine which mutex to wait for
        //      second_mutex = Other mutex        
        //      wait_for_mutex(first_mutex) //Code sits here till mutex is unlocked
        //      lock_mutex(first_mutex)
        //      wait_for_mutex(second_mutex) //Code sits here till mutex is unlocked
        //      lock_mutex(second_mutex)
        //      Remove hunter pointer from curr_room
        //      Add hunter pointer to target_room
        //      Set curr_room to target_room      
        //      Unlock mutexes

        sem_t *first_mutex;
        sem_t *second_mutex;

        int name_cmp = strncmp(hunter->curr_room->name, target_room->name, MAX_ROOM_NAME); //Compares the names of both rooms alphabetically 
        if(name_cmp > 0){
            first_mutex = &hunter->curr_room->mutex;
            second_mutex = &target_room->mutex;
        }else if(name_cmp < 0){
            first_mutex = &target_room->mutex;
            second_mutex = &hunter->curr_room->mutex;
        }
        else{
            //Do nothing if it's the same room
            return;
        }    
        
        /*
        sem_wait(first_mutex);
        printf("Waiting for sem1..\n");
        sem_wait(second_mutex);
        printf("Waiting for sem2..\n");
        */
        bool success = room_add_hunter(target_room, hunter);

        if(success){
            log_move(hunter->id, hunter->boredom, hunter->fear, hunter->curr_room->name, target_room->name, hunter->device_type);
            room_remove_hunter(hunter->curr_room, hunter);
            hunter->curr_room = target_room;
            hunter_trail_push(hunter);
        }
        
        //Unlock mutexes
        /*
        sem_post(first_mutex);

        sem_post(second_mutex);
        */
}  


/**
 * @brief Removes a hunter from the simulation, setting their exit reason
 * @param[out] hunter The hunter structure to be removed from the simulation
 * @param[in] reason The exit reason of the hunter
 */
void hunter_exit_simulation(Hunter* hunter){
    //Remove from room
    //Log exit reason
    //Set hasExited to true
    room_remove_hunter(hunter->curr_room, hunter); 
    hunter->hasExited = true;
    log_exit(hunter->id, hunter->boredom, hunter->fear, hunter->curr_room->name, hunter->device_type, hunter->exit_reason);
    hunter->curr_room = NULL;
}

/**
 * Frees all heap memory associated with a HunterCollection
 */
void hunter_collection_cleanup(HunterCollection* hc){
    for(int i = 0; i < hc->size; i++){
        free(hc->hunters[i]);
    }
    free(hc->hunters);
}

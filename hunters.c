
#include "defs.h"
#include "helpers.h"
#include <stdlib.h>
#include <string.h>

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

    if(popped == NULL){
        *room = NULL;
        return;
    }

    *room = popped->data;
    hunter->trail.top = popped->previous;
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
            int evidence_pos = hunter_check_evidence(hunter);
            if(evidence_pos > -1){
                clear_bit(&hunter->curr_room->evidence, evidence_pos);
                set_bit(&hunter->case_file->collected, evidence_pos);
                hunter->return_to_exit = true;
            }
            else{
                //A 10% chance that the hunter will return to the exit
                int chance = rand_int_threadsafe(1, 11);
                if(chance == 1){
                    hunter->return_to_exit = true;
                }
            }
        }
    }
    if(hunter->return_to_exit){
        Room *popped_room;
        hunter_trail_pop(hunter, &popped_room);

        hunter_move(hunter, popped_room); 
    }
    else if(!hunter->hasExited){
        //Select a random connected room
        int room_index = rand_int_threadsafe(0, hunter->curr_room->connections_no);
        Room* target_room = hunter->curr_room->connected_rooms[room_index];

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
        hunter_move(hunter, target_room);
    }   
}

bool hunter_check_ghost(Hunter* hunter){
    return false;
}

bool hunter_in_exit(Hunter* hunter){
    return false;
}
void hunter_check_exited(Hunter* hunter){

}
bool hunter_check_victory(Hunter* hunter){
    return false;
}
bool hunter_check_emotions(Hunter* hunter){
    return false;
}
int hunter_check_evidence(Hunter* hunter){
    return 0;
}
void hunter_move(Hunter* hunter, Room* target_room){

}  
void hunter_exit_simulation(Hunter* hunter){
    //Remove from room
    //Log exit reason
    //Set hasExited to true
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

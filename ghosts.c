
#include "defs.h"
#include "helpers.h"
#include <stdio.h>

/**
 * @brief Initialize a ghost structure and add it to the house
 * @param[in] house The house that the ghost will be added to
 * @param[out] ghost A pointer to the ghost structure to be initialized
 */
void ghost_init(House* house){
    //Set ghost type to a random type
    const enum GhostType *type_list;
    int type_list_size = get_all_ghost_types(&type_list);

    enum GhostType new_type = type_list[rand_int_threadsafe(0, type_list_size)];
    house->ghost.type = new_type;


    house->ghost.id = DEFAULT_GHOST_ID;

    
    //Set ghost to a random room
    int room_index = rand_int_threadsafe(0, house->room_count);
    house->rooms[room_index].ghost = &house->ghost;
    house->ghost.room = &house->rooms[room_index];

    house->ghost.boredom = 0;


    log_ghost_init(house->ghost.id, house->ghost.room->name, house->ghost.type);
    //printf("%s type ghost appeared in %s\n", ghost_to_string(ghost->type), ghost->room->name);


}

/**
 * @brief Goes through one loop of actions for the ghosts
 * @param[in] ghost a pointer to the Ghost structure that will take action
 */
void ghost_take_action(Ghost* ghost){
    int action;
    if(ghost_check_hunter(ghost)){
        ghost->boredom = 0;
        action = rand_int_threadsafe(0, 2);
        if(action == 0){
            ghost_haunt(ghost);
        }
        else{
            //Idle i.e. do nothing
        }
    }
    else{
        ghost->boredom++;
        if(ghost->boredom >= ENTITY_BOREDOM_MAX){
            ghost_exit(ghost);
        }
        else{
            action = rand_int_threadsafe(0, 3);
            switch(action){
                case 0:
                    ghost_haunt(ghost);
                    break;
                case 1:
                    ghost_move(ghost);
                    break;
                case 2:
                    //Do nothing otherwise
                    printf("Ghost idling\n");
                    break;
            }
        
        }
    }
}

bool ghost_check_hunter(Ghost* ghost){
    return false;
}

/**
 * @brief Leaves evidence of ghost in the room
 * @param[in] ghost The ghost structure that will leave behind evidence
 */
void ghost_haunt(Ghost* ghost){
    /*
        Choose a random evidence pos from ghost evidence byte
        If it's set (=1), check if that bit is already in the house
            if yes, do nothing
            if no, set house evidence bit
        Loop until evidence pos is set and that pos is not set in the house
    */
    
    int rand_evidence_pos = 0; //Chosen evidence bit position of evidence to be added to the room
    EvidenceByte ghost_evidence = 0;
    EvidenceByte house_evidence = 0;

    EvidenceByte type_to_evidence = (EvidenceByte)ghost->type;//Type cast GhostType to EvidenceByte

    //Only add evidence to the room if that particular rand_evidence_pos is found in ghost and isn't already in the room
    //Otherwise keep choosing a random evidence bit position of evidence
    int fail_count = -1; //The number of times evidence failed to be set for a room because the room already has that evidence
    while((!ghost_evidence || house_evidence) && fail_count <= 120 ){//fail_count > 120 (6 choose 3)x6 probably means all evidence for ghost exists in the room
        rand_evidence_pos = rand_int_threadsafe(0, (sizeof(EvidenceByte)*8)-1);

        
        ghost_evidence = get_bit(&type_to_evidence, rand_evidence_pos);
        house_evidence = get_bit(&ghost->room->evidence, rand_evidence_pos);
        fail_count++;
    }

    if(fail_count > 20){
        printf("Evidence not set. All evidence is probably here.\n");
    }
    else{
        set_bit(&ghost->room->evidence, rand_evidence_pos);
        //Log
        if(get_bit(&ghost->room->evidence, rand_evidence_pos)){
            printf("Evidence has been set successfully.\n");
            printf("Current Room [%s] evidence: ", ghost->room->name);
            for(int i = 0; i < (sizeof(EvidenceByte)*8); i++){
                printf("%d", get_bit(&ghost->room->evidence, i));
            }
            printf("\n");
        }
    }
    
}

void ghost_exit(Ghost* ghost){
    ghost->hasExited = true;
    log_ghost_exit(ghost->id, ghost->boredom, ghost->room->name);
}

void ghost_move(Ghost* ghost){
    Room* curr_room = ghost->room;
    int room_index = rand_int_threadsafe(0, curr_room->connections_no);
    Room* target_room = ghost->room->connected_rooms[room_index];
    
    //Remove ghost pointer from curr_room and add it to target_room
    //Change ghost's room pointer to target_room
    curr_room->ghost = NULL;
    target_room->ghost = ghost;

    ghost->room = target_room;

    log_ghost_move(ghost->id, ghost->boredom, curr_room->name, target_room->name);

}


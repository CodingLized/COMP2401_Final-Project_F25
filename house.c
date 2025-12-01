#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "helpers.h"




/**
 * @brief Connects the rooms a and b by adding each to the other's connected_rooms
 * @param[in,out] a A room to be connected
 * @param[in,out] b The other room to be connected
 */
void rooms_connect(Room* a, Room* b){
    a->connected_rooms[a->connections_no++] = b;
    b->connected_rooms[b->connections_no++] = a;
}


/**
 * @brief Initializes a room
 * @param[out] room The room to be initialized
 * @param[in] name The name of the room
 * @param[in] is_exit Indicates whether the room is the exit room.
 */
void room_init(Room* room, const char* name, bool is_exit){
    strncpy(room->name, name, MAX_ROOM_NAME);

    room->isExit = is_exit;

    sem_init(&room->mutex, 0, 1); //Initialze the room's mutex

    room->connections_no = room->hunter_no = 0;

    room->ghost = NULL;

    room->evidence = 0;
}

/**
 * @brief Adds a pointer to a hunter structure to the room's hunter array
 * @param[out] room The room that the hunter will be added to
 * @param[in] hunter The hunter to be added to the room
 */
bool room_add_hunter(Room* room, Hunter* hunter){

    //Check whether room is full
    if(room->hunter_no >= MAX_ROOM_OCCUPANCY){
        return false;
    }

    room->hunters[room->hunter_no] = hunter;
    room->hunter_no++;

    return true;
}

/**
 * @brief Removes a pointer to a hunter structure from the room's hunter array
 * @param[out] room The room that the hunter will be added to
 * @param[in] hunter The hunter to be added to the room
 */
void room_remove_hunter(Room* room, Hunter* hunter){
    
    if(room->hunter_no > 1){
        
        int hunter_index = -1;
        for(int i = 0; i < room->hunter_no; i ++){
            if(room->hunters[i] == hunter){//Find the index of the hunter
                hunter_index = i;
                break;
            }
        }

        for(int i = hunter_index; i < (room->hunter_no - 1); i++){//Shift all proceeding hunter pointers back, replacing the hunter of interest
                room->hunters[i] = room->hunters[i+1];
        }

        room->hunter_no--;
    }
    else if (room->hunter_no == 1){//No need to loop if there's only one hunter. A future add to the hunter array will overwrite the hunter pointer
        room->hunter_no--;
    }
    //Do nothing if room->hunter_no == 0, i.e. hunter was only initialized in the Van.
}

/**
 * @brief Frees all associated heap data of the mutexes of each room in the house
 * @param house A pointer to a House structure containing rooms with mutexes. 
 */
void house_destroy_mutexes(House* house){
    for(int i = 0; i < house->room_count; i++){
        sem_destroy(&house->rooms[i].mutex);
    }
}
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

    room->connections_no = room->hunter_no = 0;

    room->ghost = NULL;



}
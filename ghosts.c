
#include "defs.h"
#include "helpers.h"

void ghost_init(Ghost* ghost, House* house){
    //Set ghost type to a random type
    enum GhostType *type_list;
    int type_list_size = get_all_ghost_types(&type_list);

    enum GhostType new_type = type_list[rand_int_threadsafe(0, type_list_size)];
    ghost->type = new_type;


    ghost->id = DEFAULT_GHOST_ID;

    
    //Set ghost to a random room
    int room_index = rand_int_threadsafe(0, house->room_count);
    house->rooms[room_index].ghost = ghost;
    ghost->room = &house->rooms[room_index];

    ghost->boredom = 0;

    log_ghost_init(ghost->id, ghost->room, ghost->type);
    printf("%s type ghost appeared in %s\n", ghost_to_string(ghost->type));


}

void ghost_check_hunter(Ghost* ghost){

}
void ghost_take_action(Ghost* ghost){

}
void ghost_haunt(Ghost* ghost){

}
void ghost_exit(Ghost* ghost){

}
void ghost_move(Ghost* ghost){
    
}


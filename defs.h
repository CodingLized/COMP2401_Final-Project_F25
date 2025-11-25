#ifndef DEFS_H
#define DEFS_H

#include <stdbool.h>
#include <semaphore.h>
#include <pthread.h>

/*
    You are free to rename all of the types and functions defined here.

    The ghost ID must remain the same for the validator to work correctly.
*/

#define MAX_ROOM_NAME 64
#define MAX_HUNTER_NAME 64
#define MAX_ROOMS 24
#define MAX_ROOM_OCCUPANCY 8
#define MAX_CONNECTIONS 8
#define ENTITY_BOREDOM_MAX 15
#define HUNTER_FEAR_MAX 15
#define DEFAULT_GHOST_ID 68057

typedef unsigned char EvidenceByte; // Just giving a helpful name to unsigned char for evidence bitmasks
typedef enum EvidenceType EvidenceType;
typedef enum GhostType GhostType;
typedef enum LogReason LogReason;
typedef struct Room Room;
typedef struct RoomStack RoomStack;
typedef struct RoomNode RoomNode;
typedef struct CaseFile CaseFile;
typedef struct Ghost Ghost;
typedef struct House House;
typedef struct Hunter Hunter;
typedef struct HunterCollection HunterCollection;



enum LogReason {
    LR_EVIDENCE = 0,
    LR_BORED = 1,
    LR_AFRAID = 2
};

enum EvidenceType {
    EV_EMF          = 1 << 0,
    EV_ORBS         = 1 << 1,
    EV_RADIO        = 1 << 2,
    EV_TEMPERATURE  = 1 << 3,
    EV_FINGERPRINTS = 1 << 4,
    EV_WRITING      = 1 << 5,
    EV_INFRARED     = 1 << 6,
};

enum GhostType {
    GH_POLTERGEIST  = EV_FINGERPRINTS | EV_TEMPERATURE | EV_WRITING,
    GH_THE_MIMIC    = EV_FINGERPRINTS | EV_TEMPERATURE | EV_RADIO,
    GH_HANTU        = EV_FINGERPRINTS | EV_TEMPERATURE | EV_ORBS,
    GH_JINN         = EV_FINGERPRINTS | EV_TEMPERATURE | EV_EMF,
    GH_PHANTOM      = EV_FINGERPRINTS | EV_INFRARED    | EV_RADIO,
    GH_BANSHEE      = EV_FINGERPRINTS | EV_INFRARED    | EV_ORBS,
    GH_GORYO        = EV_FINGERPRINTS | EV_INFRARED    | EV_EMF,
    GH_BULLIES      = EV_FINGERPRINTS | EV_WRITING     | EV_RADIO,
    GH_MYLING       = EV_FINGERPRINTS | EV_WRITING     | EV_EMF,
    GH_OBAKE        = EV_FINGERPRINTS | EV_ORBS        | EV_EMF,
    GH_YUREI        = EV_TEMPERATURE  | EV_INFRARED    | EV_ORBS,
    GH_ONI          = EV_TEMPERATURE  | EV_INFRARED    | EV_EMF,
    GH_MOROI        = EV_TEMPERATURE  | EV_WRITING     | EV_RADIO,
    GH_REVENANT     = EV_TEMPERATURE  | EV_WRITING     | EV_ORBS,
    GH_SHADE        = EV_TEMPERATURE  | EV_WRITING     | EV_EMF,
    GH_ONRYO        = EV_TEMPERATURE  | EV_RADIO       | EV_ORBS,
    GH_THE_TWINS    = EV_TEMPERATURE  | EV_RADIO       | EV_EMF,
    GH_DEOGEN       = EV_INFRARED     | EV_WRITING     | EV_RADIO,
    GH_THAYE        = EV_INFRARED     | EV_WRITING     | EV_ORBS,
    GH_YOKAI        = EV_INFRARED     | EV_RADIO       | EV_ORBS,
    GH_WRAITH       = EV_INFRARED     | EV_RADIO       | EV_EMF,
    GH_RAIJU        = EV_INFRARED     | EV_ORBS        | EV_EMF,
    GH_MARE         = EV_WRITING      | EV_RADIO       | EV_ORBS,
    GH_SPIRIT       = EV_WRITING      | EV_RADIO       | EV_EMF,
};

struct CaseFile {
    EvidenceByte collected; // Union of all of the evidence bits collected between all hunters
    bool         solved;    // True when >=3 unique bits set
    sem_t        mutex;     // Used for synchronizing both fields when multithreading
};

// Implement here based on the requirements, should all be allocated to the House structure
struct Room {
    char name[MAX_ROOM_NAME];
    Room* connected_rooms[MAX_CONNECTIONS];
    int connections_no;
    Hunter* hunters[MAX_ROOM_OCCUPANCY];
    int hunter_no;
    Ghost* ghost;
    bool isExit;
    EvidenceByte evidence;
    sem_t mutex;

};

struct RoomStack {
    RoomNode* top;
};

struct RoomNode {
    Room* data;
    RoomNode* previous;
};

// Implement here based on the requirements, should be allocated to the House structure
struct Ghost {
    int id;
    GhostType type;
    Room *room;
    int boredom;
    bool hasExited;
};

struct Hunter {
    char name[MAX_HUNTER_NAME];
    int id;
    Room* curr_room;
    CaseFile* case_file;
    EvidenceType device_type;
    RoomStack trail;
    int fear;
    int boredom;
    LogReason exit_reason;
    bool hasExited;
};

struct HunterCollection {
    Hunter* *hunters;
    int size;
};


// Can be either stack or heap allocated
struct House {
    Room* starting_room; // Needed by house_populate_rooms, but can be adjusted to suit your needs.
    Room rooms[MAX_ROOMS];
    int room_count;
    HunterCollection hunterCollection;
    CaseFile case_file;
    Ghost ghost;
};

/* The provided `house_populate_rooms()` function requires the following functions.
   You are free to rename them and change their parameters and modify house_populate_rooms()
   as needed as long as the house has the correct rooms and connections after calling it.
*/


void set_bit(unsigned char* byte, int pos);
int get_bit(const unsigned char* byte, int pos);


void room_init(Room* room, const char* name, bool is_exit);
void rooms_connect(Room* a, Room* b); // Bidirectional connection

//Initializers
void ghost_init(Ghost* ghost, House* house);
void hunter_collection_init(HunterCollection* *hc);
void hunter_init(House* house, const char* name, int id);
void hunter_trail_init(Hunter* hunter);



//Ghost Functions
void ghost_check_hunter(Ghost* ghost);
void ghost_take_action(Ghost* ghost);
void ghost_haunt(Ghost* ghost);
void ghost_exit(Ghost* ghost);
void ghost_move(Ghost* ghost);

//Hunter Functions
void hunter_check_ghost(Hunter* hunter);
void hunter_check_exited(Hunter* hunter);
void hunter_check_victory(Hunter* hunter);
void hunter_check_emotions(Hunter* hunter);
void hunter_check_evidence(Hunter* hunter);
void hunter_move(Hunter* hunter);  
void hunter_exit(Hunter* hunter);

void select_rand_device(EvidenceType* device);

void hunter_trail_push(Hunter* hunter);
void hunter_trail_pop(Hunter* hunter, Room* *room);

void hunter_collection_add(Hunter* hunter, House* house);

//Heap Cleanup
void hunter_collection_cleanup(HunterCollection* hc);


#endif // DEFS_H

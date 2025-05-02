#ifndef COMMON_H
#define COMMON_H

// Maximum number of stations that can be part of the Ethernet simulation
#define MAX_STATIONS 10

// Maximum size of a frame (not used for payload but could be extended)
#define MAX_FRAME_SIZE 1024

// Time duration of a single time slot (in milliseconds)
#define SLOT_DURATION_MS 100

// Maximum number of retries due to collisions
#define MAX_COLLISIONS 16

// Path for the UNIX domain socket used by the CBP (central bus)
#define CBP_SOCKET_PATH "/tmp/ethernet_cbp.sock"

// Structure representing a frame part to be sent from one station to another
typedef struct {
    int src_station;      // ID of the sending station
    int dst_station;      // ID of the receiving station
    int frame_number;     // Frame number being transmitted
    int part;             // Part number (1 or 2)
    char payload[256];    // Data payload of the frame part
} FramePart;

#endif

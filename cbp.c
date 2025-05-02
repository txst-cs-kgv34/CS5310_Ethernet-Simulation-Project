// cbp.c - Communication Bus Process
// This process simulates a central bus that receives frame parts from multiple stations,
// detects collisions, and logs communication activities.

#include "common.h"
#include "wrapsock.h"
#include <stdlib.h>
#include <fcntl.h>

// Buffer to hold the first received part of a frame
FramePart buffer;

// Indicates whether buffer is in use (0 = empty, 1 = occupied)
int buffer_state = 0;

// ID of the station that sent the first part (used for collision detection)
int first_sender = -1;

// File descriptor for the log file
int logfd;

/**
 * Logs a message to cbp.log.
 * @param msg The message string to be logged.
 */
void log_event(const char *msg) {
    dprintf(logfd, "%s\n", msg);
    fflush(stdout);
}

int main() {
    struct sockaddr_un addr;

    // Create a UNIX domain datagram socket
    int sockfd = Socket(AF_UNIX, SOCK_DGRAM, 0);

    // Ensure the socket path doesn't already exist
    unlink(CBP_SOCKET_PATH);

    // Set up address structure
    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, CBP_SOCKET_PATH);

    // Bind the socket to the specified address
    Bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    // Open log file
    logfd = open("logs/cbp.log", O_WRONLY | O_CREAT | O_APPEND, 0666);
    log_event("CBP started");

    FramePart part;
    while (1) {
        // Wait and receive a frame part from any station
        Recvfrom(sockfd, &part, sizeof(part), 0, NULL, NULL);

        // Log the received frame part
        char msg[256];
        snprintf(msg, sizeof(msg), "Receive part %d of frame %d from Station %d, to Station %d",
                 part.part, part.frame_number, part.src_station, part.dst_station);
        log_event(msg);

        // Validate frame part number
        if (part.part != 1 && part.part != 2) {
            snprintf(msg, sizeof(msg), "Invalid part number %d from Station %d", part.part, part.src_station);
            log_event(msg);
            continue;
        }

        // No frame is currently buffered and part 1 is received — store it
        if (buffer_state == 0 && part.part == 1) {
            memcpy(&buffer, &part, sizeof(part));
            buffer_state = 1;
            first_sender = part.src_station;

        // Part 2 received and matches the stored part 1 — transfer both parts
        } else if (buffer_state == 1 && part.part == 2 &&
                   part.src_station == buffer.src_station &&
                   part.frame_number == buffer.frame_number) {

            snprintf(msg, sizeof(msg), "Transfer part 1 of frame %d from Station %d, to Station %d",
                     buffer.frame_number, buffer.src_station, buffer.dst_station);
            log_event(msg);

            snprintf(msg, sizeof(msg), "Transfer part 2 of frame %d from Station %d, to Station %d",
                     part.frame_number, part.src_station, part.dst_station);
            log_event(msg);

            buffer_state = 0;
            first_sender = -1;

        // Collision: two different stations sent part 1 at the same time
        } else if (buffer_state == 1 && part.part == 1) {
            snprintf(msg, sizeof(msg), "Inform Station %d, Station %d, a collision",
                     first_sender, part.src_station);
            log_event(msg);
            log_event("A collision informed, wait for 1 time slot");

            buffer_state = 0;
            first_sender = -1;

        // Part 2 received but no matching part 1 — discard
        } else if (buffer_state == 1 && part.part == 2) {
            snprintf(msg, sizeof(msg), "Dropped unmatched part 2 of frame %d from Station %d",
                     part.frame_number, part.src_station);
            log_event(msg);

        } else if (buffer_state == 0 && part.part == 2) {
            snprintf(msg, sizeof(msg), "Dropped unmatched part 2 of frame %d from Station %d",
                     part.frame_number, part.src_station);
            log_event(msg);
        }
    }

    return 0;
}

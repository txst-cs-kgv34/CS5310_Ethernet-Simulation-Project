// CBP: Communication Bus Process
#include "common.h"
#include "wrapsock.h"
#include<stdlib.h>
#include <fcntl.h>

FramePart buffer;
int buffer_state = 0;
int first_sender = -1;
int logfd;

void log_event(const char *msg) {
    dprintf(logfd, "%s\n", msg);
    fflush(stdout);
}

int main() {
    struct sockaddr_un addr;
    int sockfd = Socket(AF_UNIX, SOCK_DGRAM, 0);
    unlink(CBP_SOCKET_PATH);

    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, CBP_SOCKET_PATH);
    Bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    logfd = open("logs/cbp.log", O_WRONLY | O_CREAT | O_APPEND, 0666);
    log_event("CBP started");

    FramePart part;
    while (1) {
        Recvfrom(sockfd, &part, sizeof(part), 0, NULL, NULL);

        char msg[256];
        snprintf(msg, sizeof(msg), "Receive part %d of frame %d from Station %d, to Station %d",
                 part.part, part.frame_number, part.src_station, part.dst_station);
        log_event(msg);

        if (part.part != 1 && part.part != 2) {
            snprintf(msg, sizeof(msg), "Invalid part number %d from Station %d", part.part, part.src_station);
            log_event(msg);
            continue;
        }
        
        if (buffer_state == 0 && part.part == 1) {
            // First valid part received
            memcpy(&buffer, &part, sizeof(part));
            buffer_state = 1;
            first_sender = part.src_station;

        } else if (buffer_state == 1 && part.part == 2 &&
                   part.src_station == buffer.src_station &&
                   part.frame_number == buffer.frame_number) {
            // Matching part 2 received
            snprintf(msg, sizeof(msg), "Transfer part 1 of frame %d from Station %d, to Station %d",
                     buffer.frame_number, buffer.src_station, buffer.dst_station);
            log_event(msg);

            snprintf(msg, sizeof(msg), "Transfer part 2 of frame %d from Station %d, to Station %d",
                     part.frame_number, part.src_station, part.dst_station);
            log_event(msg);

            buffer_state = 0;
            first_sender = -1;

        } else if (buffer_state == 1 && part.part == 1) {
            // Two stations sending part 1 simultaneously
            snprintf(msg, sizeof(msg), "Inform Station %d, Station %d, a collision",
                     first_sender, part.src_station);
            log_event(msg);
            log_event("A collision informed, wait for 1 time slot");

            buffer_state = 0;
            first_sender = -1;

        } else if (buffer_state == 1 && part.part == 2) {
            // Orphaned part 2 — discard, no matching part 1
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

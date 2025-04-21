// CBP: Communication Bus Process
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

FramePart buffer;
int buffer_state = 0; // 0=empty, 1=part1, 2=part2
int stations[MAX_STATIONS];
int logfd;

void log_event(const char *msg) {
    dprintf(logfd, "%s\n", msg);
    fflush(stdout);
}

int main() {
    struct sockaddr_un addr;
    int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    unlink(CBP_SOCKET_PATH);

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, CBP_SOCKET_PATH);
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    logfd = open("logs/cbp.log", O_WRONLY | O_CREAT | O_APPEND, 0666);
    log_event("CBP started");

    FramePart part;
    while (1) {
        recvfrom(sockfd, &part, sizeof(part), 0, NULL, NULL);

        char msg[256];
        snprintf(msg, sizeof(msg), "Received part %d of frame %d from Station %d to Station %d",
            part.part, part.frame_number, part.src_station, part.dst_station);
        log_event(msg);

        if (buffer_state == 0 && part.part == 1) {
            memcpy(&buffer, &part, sizeof(part));
            buffer_state = 1;
        } else if (buffer_state == 1 && part.part == 2 &&
                   part.src_station == buffer.src_station &&
                   part.frame_number == buffer.frame_number) {
            buffer_state = 0;
            snprintf(msg, sizeof(msg), "Delivered frame %d to Station %d",
                     part.frame_number, part.dst_station);
            log_event(msg);
        } else {
            // Collision
            snprintf(msg, sizeof(msg), "Collision detected on frame %d from Station %d",
                     part.frame_number, part.src_station);
            log_event(msg);
            buffer_state = 0;
        }
    }
    return 0;
}

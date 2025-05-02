// station.c - Station Process
// This program simulates a station on the Ethernet bus. Each station reads an input
// file of frames to send, sends frame parts to the CBP, and handles collisions using
// Binary Exponential Backoff (BEBO) strategy.

#include "common.h"
#include "wrapsock.h"
#include <fcntl.h>

/**
 * Waits for a given number of time slots. Each slot corresponds to a fixed duration.
 * @param slots Number of time slots to wait.
 */
void wait_slot(int slots) {
    usleep(slots * SLOT_DURATION_MS * 1000);
}

/**
 * Logs a message to a specific station log file.
 * @param id Station ID (used to name the log file)
 * @param msg Message to log
 */
void log_event(int id, const char *msg) {
    char filename[64];
    snprintf(filename, sizeof(filename), "logs/stationprocess%d.log", id);
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
    dprintf(fd, "%s\n", msg);
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <station_id> <input_file>\n", argv[0]);
        exit(1);
    }

    int id = atoi(argv[1]);
    FILE *f = fopen(argv[2], "r");
    if (!f) exit(1);

    // Set up UNIX domain datagram socket
    int sockfd = Socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un addr;
    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, CBP_SOCKET_PATH);

    int frame = 1;
    int collision_count = 0;
    char line[128];

    // Process each line in the input file
    while (fgets(line, sizeof(line), f)) {
        int dst;
        sscanf(line, "Frame %*d, To Station %d", &dst);

        // Construct frame parts
        FramePart p1 = {id, dst, frame, 1, "Data1"};
        FramePart p2 = {id, dst, frame, 2, "Data2"};

        // Send part 1 of the frame
        char msg[128];
        snprintf(msg, sizeof(msg), "Send part 1 of frame %d to Station %d", frame, dst);
        log_event(id, msg);
        Sendto(sockfd, &p1, sizeof(p1), 0, (struct sockaddr*)&addr, sizeof(addr));
        wait_slot(1);  // wait before sending part 2

        // Simulate collision probability: 1 in 5 chance to simulate a collision
        if (rand() % 5 == 0) {
            int slots = rand() % (1 << (collision_count < MAX_COLLISIONS ? collision_count : MAX_COLLISIONS));
            snprintf(msg, sizeof(msg), "A collision informed, wait for %d time slot", slots);
            log_event(id, msg);
            wait_slot(slots);
            collision_count++;
            continue; // retry same frame
        }

        // Optional: add random jitter before part 2
        wait_slot(rand() % 3);

        // Send part 2 of the frame
        snprintf(msg, sizeof(msg), "Send part 2 of frame %d to Station %d", frame, dst);
        log_event(id, msg);
        Sendto(sockfd, &p2, sizeof(p2), 0, (struct sockaddr*)&addr, sizeof(addr));

        // Move to next frame
        frame++;
        collision_count = 0;  // reset on successful send
        wait_slot(1);
    }

    fclose(f);
    return 0;
}

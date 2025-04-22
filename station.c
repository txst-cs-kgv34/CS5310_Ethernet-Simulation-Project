// STATION PROCESS
#include "common.h"
#include "wrapsock.h"
#include <fcntl.h>

void wait_slot(int slots) {
    usleep(slots * SLOT_DURATION_MS * 1000);
}

void log_event(int id, const char *msg) {
    char filename[64];
    snprintf(filename, sizeof(filename), "logs/station%d.log", id);
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

    int sockfd = Socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un addr;
    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, CBP_SOCKET_PATH);

    int frame = 1, collision_count = 0;
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        int dst;
        sscanf(line, "Frame %*d, To Station %d", &dst);

        FramePart p1 = {id, dst, frame, 1, "Data1"};
        FramePart p2 = {id, dst, frame, 2, "Data2"};

        char msg[128];
        snprintf(msg, sizeof(msg), "Send part 1 of frame %d to Station %d", frame, dst);
        log_event(id, msg);
        Sendto(sockfd, &p1, sizeof(p1), 0, (struct sockaddr*)&addr, sizeof(addr));
        wait_slot(1);

        if (rand() % 5 == 0) {
            int slots = rand() % (1 << (collision_count < 10 ? collision_count : 10));
            snprintf(msg, sizeof(msg), "A collision informed, wait for %d time slot", slots);
            log_event(id, msg);
            wait_slot(slots);
            collision_count++;
            continue;
        }

        snprintf(msg, sizeof(msg), "Send part 2 of frame %d to Station %d", frame, dst);
        log_event(id, msg);
        Sendto(sockfd, &p2, sizeof(p2), 0, (struct sockaddr*)&addr, sizeof(addr));

        frame++;
        collision_count = 0;
        wait_slot(1);
    }

    fclose(f);
    return 0;
}

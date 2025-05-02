// wrapsock.c - Socket wrapper implementation
// Provides error-checked wrappers for socket system calls used in the Ethernet simulation.

#include "wrapsock.h"

// Wrapper for socket(). Creates a socket and exits with error if failed.
int Socket(int domain, int type, int protocol) {
    int sockfd;
    if ((sockfd = socket(domain, type, protocol)) < 0) {
        perror("socket error");
        exit(1);
    }
    return sockfd;
}


// Wrapper for bind(). Binds a socket to a given address, with error handling.
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) < 0) {
        perror("bind error");
        exit(1);
    }
}


// Wrapper for sendto(). Sends a datagram to a specific address.
void Sendto(int sockfd, const void *buf, size_t len, int flags,
            const struct sockaddr *dest_addr, socklen_t addrlen) {
    if (sendto(sockfd, buf, len, flags, dest_addr, addrlen) < 0) {
        perror("sendto error");
        exit(1);
    }
}

// Wrapper for recvfrom(). Receives a datagram from a socket.
void Recvfrom(int sockfd, void *buf, size_t len, int flags,
              struct sockaddr *src_addr, socklen_t *addrlen) {
    if (recvfrom(sockfd, buf, len, flags, src_addr, addrlen) < 0) {
        perror("recvfrom error");
        exit(1);
    }
}
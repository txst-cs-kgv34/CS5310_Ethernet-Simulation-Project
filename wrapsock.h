#ifndef WRAPSOCK_H
#define WRAPSOCK_H

#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/**
 * Wrapper for socket(). Terminates program if socket creation fails.
 * @param domain Address family (e.g., AF_UNIX)
 * @param type Socket type (e.g., SOCK_DGRAM)
 * @param protocol Typically 0 for default
 * @return File descriptor for the created socket
 */
int Socket(int domain, int type, int protocol);

/**
 * Wrapper for bind(). Terminates program if bind fails.
 * @param sockfd Socket file descriptor
 * @param addr Pointer to sockaddr structure
 * @param addrlen Length of the address structure
 */
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/**
 * Wrapper for sendto(). Terminates program if sending fails.
 * @param sockfd Socket file descriptor
 * @param buf Pointer to the data buffer
 * @param len Length of the buffer
 * @param flags Send flags (usually 0)
 * @param dest_addr Destination address
 * @param addrlen Length of the destination address
 */
void Sendto(int sockfd, const void *buf, size_t len, int flags,
            const struct sockaddr *dest_addr, socklen_t addrlen);

/**
 * Wrapper for recvfrom(). Terminates program if receiving fails.
 * @param sockfd Socket file descriptor
 * @param buf Pointer to buffer to store received data
 * @param len Maximum number of bytes to read
 * @param flags Flags (usually 0)
 * @param src_addr Source address storage (optional)
 * @param addrlen Pointer to length of src_addr structure
 */
void Recvfrom(int sockfd, void *buf, size_t len, int flags,
              struct sockaddr *src_addr, socklen_t *addrlen);

#endif

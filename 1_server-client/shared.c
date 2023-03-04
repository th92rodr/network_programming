#include "shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

int send_message(int socket_fd, char *message, int *total_bytes_sent) {
    const int FLAGS = 0;

    *total_bytes_sent = 0;

    int message_size = strlen(message);
    if (send(socket_fd, &message_size, sizeof(int), FLAGS) == -1) {
        perror("Error sending message");
        return -1;
    }

    int bytes_left = message_size;
    while (*total_bytes_sent < message_size) {
        int bytes_sent = send(socket_fd, message+(*total_bytes_sent), bytes_left, FLAGS);
        if (bytes_sent == -1) {
            perror("Error sending message");
            return -1;
        }
        *total_bytes_sent += bytes_sent;
        bytes_left -= bytes_sent;
    }

    return 0;
}

int read_message(int socket_fd, char **message, int *total_bytes_read) {
    const int FLAGS = 0;

    *total_bytes_read = 0;

    int message_size;
    if (recv(socket_fd, &message_size, sizeof(int), FLAGS) == -1) {
        perror("Error receiving message size");
        return -1;
    }

    char *buffer = (char*) malloc(message_size+1 * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating buffer\n");
        return -1;
    }
    memset(buffer, '\0', message_size+1 * sizeof(char));

    int bytes_left = message_size;
    while (*total_bytes_read < message_size) {
        int bytes_read = recv(socket_fd, buffer+(*total_bytes_read), bytes_left, FLAGS);
        if (bytes_read == -1) {
            perror("Error receiving message");
            return -1;
        }
        if (bytes_read == 0) {
            perror("Connection closed");
            return -1;
        }
        *total_bytes_read += bytes_read;
        bytes_left -= bytes_read;
    }

    *message = buffer;

    return 0;
}

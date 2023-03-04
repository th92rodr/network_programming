#include "../1_server-client/shared.h"
#include "../1_server-client/server_functions.h"
#include "poll.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

int create_file_descriptors_poll(int socket_fd, int *connections_num, struct pollfd **fds_poll) {
    int connections = 1;
    struct pollfd *poll_file_descriptors = (struct pollfd*) malloc(connections * sizeof(struct pollfd*));
    if (poll_file_descriptors == NULL) {
        return -1;
    }

    poll_file_descriptors[0].fd = socket_fd;
    poll_file_descriptors[0].events = POLLIN;

    *connections_num = connections;
    *fds_poll = poll_file_descriptors;

    return 0;
}

int evaluate_event(int socket_fd, int *connections, struct pollfd **fds_poll) {
    for (int i = 0; i < *connections; i++) {
        if ((*fds_poll)[i].revents & POLLIN) {
            if ((*fds_poll)[i].fd == socket_fd) {
                handle_new_connection(socket_fd, connections, fds_poll);
                break;

            } else {
                handle_same_connection((*fds_poll)[i].fd, i, connections, fds_poll);
                break;
            }
        }
    }
}

static int add_connection_to_poll(int fd, int *connections, struct pollfd **fds_poll) {
    (*connections)++;

    *fds_poll = realloc(*fds_poll, (*connections) * sizeof(struct pollfd*));
    if (*fds_poll == NULL) {
        (*connections)--;
        fprintf(stderr, "Error reallocating poll\n");
        return -1;
    }

    (*fds_poll)[*connections-1].fd = fd;
    (*fds_poll)[*connections-1].events = POLLIN;

    return 0;
}

static int remove_connection_from_poll(int fd_index, int *connections, struct pollfd **fds_poll) {
    (*fds_poll)[fd_index] = (*fds_poll)[(*connections)-1];

    (*connections)--;

    *fds_poll = realloc(*fds_poll, (*connections) * sizeof(struct pollfd*));
    if (*fds_poll == NULL) {
        (*connections)++;
        fprintf(stderr, "Error reallocating poll\n");
        return -1;
    }
}

static void handle_new_connection(int socket_fd, int *connections, struct pollfd **fds_poll) {
    int request_socket_fd;
    if (income_connection(socket_fd, &request_socket_fd) == -1) {
        return;
    }
    // printf("New connection: %d\n", request_socket_fd);

    if (add_connection_to_poll(request_socket_fd, connections, fds_poll) == -1) {
        return;
    }
}

static void handle_same_connection(int request_socket_fd, int fd_index, int *connections, struct pollfd **fds_poll) {
    // printf("Same connection: %d\n", request_socket_fd);

    char *request_message;
    int bytes_read;
    if (read_message(request_socket_fd, &request_message, &bytes_read) == -1) {
        printf("Only %d bytes were read\n", bytes_read);
    }
    printf("Message received: %s\n", request_message);
    free(request_message);

    char *response_message = "Hello back!";
    int bytes_sent;
    if (send_message(request_socket_fd, response_message, &bytes_sent) == -1) {
        printf("Only %d bytes were sent\n", bytes_sent);
    }

    close(request_socket_fd);
    if (remove_connection_from_poll(fd_index, connections, fds_poll) == -1) {
        return;
    }
}

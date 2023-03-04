/*
** polling stream socket server
*/

#include "../1_server-client/server_functions.h"
#include "poll.h"

#include <stdio.h>
#include <poll.h>

int main(int argc, char *argv[]) {
    char *port;
    parse_arguments(argc, argv, &port);

    int socket_fd;
    if (create_socket_file_descriptor(port, &socket_fd) == -1) {
        return -1;
    }

    int connections;
    struct pollfd *fds_poll;
    if (create_file_descriptors_poll(socket_fd, &connections, &fds_poll) == -1) {
        return -1;
    }

    printf("Waiting for connections...\n\n");

    while (1) {
        const int TIMEOUT = -1;

        int events = poll(fds_poll, connections, TIMEOUT);
        if (events == -1) {
            perror("Error polling");
            return -1;
        }
        if (events == 0) {
            continue;
        }

        evaluate_event(socket_fd, &connections, &fds_poll);
    }

    return 0;
}

/*
** http server
*/

#include "http.h"
#include "../1_server-client/server_functions.h"

#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *port;
    parse_arguments(argc, argv, &port);

    int socket_fd;
    if (create_socket_file_descriptor(port, &socket_fd) == -1) {
        return -1;
    }

    printf("Waiting for connections...\n\n");

    while (1) {
        int request_socket_fd;
        if (income_connection(socket_fd, &request_socket_fd) == -1) {
            continue;
        }

        handle_request(request_socket_fd);
        build_response(request_socket_fd);

        close(request_socket_fd);
    }

    return 0;
}

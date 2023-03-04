/*
** stream socket server
*/

#include "shared.h"
#include "server_functions.h"

#include <stdio.h>
#include <stdlib.h>
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
    }

    return 0;
}

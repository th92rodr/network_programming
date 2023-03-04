/*
** stream socket client
*/

#include "shared.h"
#include "client_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *host, *port;
    parse_arguments(argc, argv, &host, &port);

    int socket_fd;
    if (establish_connection(host, port, &socket_fd) == -1) {
        return -1;
    }

    char *request_message = "Hello!";
    int bytes_sent;
    if (send_message(socket_fd, request_message, &bytes_sent) == -1) {
        printf("Only %d bytes were sent\n", bytes_sent);
    }

    char *response_message;
    int bytes_read;
    if (read_message(socket_fd, &response_message, &bytes_read) == -1) {
        printf("Only %d bytes were read\n", bytes_read);
    }
    printf("Message received: %s\n", response_message);
    free(response_message);

    close(socket_fd);

    return 0;
}

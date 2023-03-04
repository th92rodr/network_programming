#include "client_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "8080"

void parse_arguments(int argc, char *argv[], char **host, char **port) {
    *host = DEFAULT_HOST;
    *port = DEFAULT_PORT;

    int opt;
    while ((opt = getopt(argc, argv, "h:p:")) != -1) {
        switch (opt) {
            case 'h':
                *host = optarg;
                break;
            case 'p':
                *port = optarg;
                break;
            default:
                break;
        }
    }

    // printf("Host %s\n", *host);
    // printf("Port %s\n", *port);
}

int establish_connection(char *host, char *port, int *socket_fd) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int error;
    struct addrinfo *server_info;
    if ((error = getaddrinfo(host, port, &hints, &server_info)) != 0) {
        fprintf(stderr, "Error getting address info: %s\n", gai_strerror(error));
        return -1;
    }

    struct addrinfo *current;
    int socket_file_descriptor;
    for (current = server_info; current != NULL; current = current->ai_next) {
        if ((socket_file_descriptor = socket(current->ai_family, current->ai_socktype, current->ai_protocol)) == -1) {
            perror("Error getting socket file descriptor");
            continue;
        }

        if (connect(socket_file_descriptor, current->ai_addr, current->ai_addrlen) == -1) {
            close(socket_file_descriptor);
            perror("Error connecting socket file descriptor");
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    if (current == NULL) {
        fprintf(stderr, "Failed to connect\n");
        return -1;
    }

    log_connection_established(current->ai_addr);

    *socket_fd = socket_file_descriptor;

    return 0;
}

void log_connection_established(struct sockaddr *server_address) {
    printf("Connection established");

    if (server_address->sa_family == AF_INET) {
        struct sockaddr_in *ipv4_address = (struct sockaddr_in*) &server_address;
        char ipv4[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipv4_address->sin_addr.s_addr), ipv4, INET_ADDRSTRLEN);
        printf(" [%s]\n", ipv4);

    } else {
        struct sockaddr_in6 *ipv6_address = (struct sockaddr_in6*) &server_address;
        char ipv6[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(ipv6_address->sin6_addr.s6_addr), ipv6, INET6_ADDRSTRLEN);
        printf(" [%s]\n", ipv6);
    }
}

/*
** stream socket server
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define DEFAULT_PORT "8080"
#define SOCKET_QUEUE_SIZE 10

void parse_arguments(int argc, char *argv[], char **port);
int create_socket_file_descriptor(char *port, int *socket_fd);
int income_connection(int socket_fd, int *request_socket_fd);
void log_connection_received(struct sockaddr_storage client_address);
int send_message(int socket_fd, char *message, int *total_bytes_sent);
int read_message(int socket_fd, char **message, int *total_bytes_read);

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

void parse_arguments(int argc, char *argv[], char **port) {
    *port = DEFAULT_PORT;

    int opt;
    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                *port = optarg;
                break;
            default:
                break;
        }
    }

    // printf("Port %s\n", *port);
}

int create_socket_file_descriptor(char *port, int *socket_fd) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int error;
    struct addrinfo *server_info;
    if ((error = getaddrinfo(NULL, port, &hints, &server_info)) != 0) {
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

        int yes = 1;
        if (setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
			perror("Error reusing socket");
			return -1;
		}

        if (bind(socket_file_descriptor, current->ai_addr, current->ai_addrlen) == -1) {
            close(socket_file_descriptor);
            perror("Error binding socket file descriptor");
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    if (current == NULL) {
        fprintf(stderr, "Failed to bind socket\n");
        return -1;
    }

    if (listen(socket_file_descriptor, SOCKET_QUEUE_SIZE) == -1) {
        perror("Error listening socket");
        return -1;
    }

    *socket_fd = socket_file_descriptor;

    return 0;
}

int income_connection(int socket_fd, int *request_socket_fd) {
    struct sockaddr_storage client_address;
    socklen_t address_size = sizeof client_address;

    int request_file_descriptor = accept(socket_fd, (struct sockaddr*) &client_address, &address_size);
    if (request_file_descriptor == -1) {
        perror("Error accepting connection");
        return -1;
    }

    log_connection_received(client_address);

    *request_socket_fd = request_file_descriptor;

    return 0;
}

void log_connection_received(struct sockaddr_storage client_address) {
    printf("Connection received");

    if (client_address.ss_family == AF_INET) {
        struct sockaddr_in *ipv4_address = (struct sockaddr_in*) &client_address;
        char ipv4[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipv4_address->sin_addr.s_addr), ipv4, INET_ADDRSTRLEN);
        printf(" [%s]\n", ipv4);

    } else {
        struct sockaddr_in6 *ipv6_address = (struct sockaddr_in6*) &client_address;
        char ipv6[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(ipv6_address->sin6_addr.s6_addr), ipv6, INET6_ADDRSTRLEN);
        printf(" [%s]\n", ipv6);
    }
}

int send_message(int socket_fd, char *message, int *total_bytes_sent) {
    int FLAGS = 0;

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
    int FLAGS = 0;

    *total_bytes_read = 0;

    int message_size;
    if (recv(socket_fd, &message_size, sizeof(int), FLAGS) == -1) {
        perror("Error receiving message size");
        return -1;
    }

    char *buffer = (char*) malloc(message_size+1 * sizeof(char));
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

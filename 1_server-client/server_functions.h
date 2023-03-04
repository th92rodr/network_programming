#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include <sys/socket.h>

void parse_arguments(int argc, char *argv[], char **port);
int create_socket_file_descriptor(char *port, int *socket_fd);
int income_connection(int socket_fd, int *request_socket_fd);
void log_connection_received(struct sockaddr_storage client_address);

#endif
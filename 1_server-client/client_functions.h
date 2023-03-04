#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H

#include <sys/socket.h>

void parse_arguments(int argc, char *argv[], char **host, char **port);
int establish_connection(char *host, char *port, int *socket_fd);
void log_connection_established(struct sockaddr *server_address);

#endif
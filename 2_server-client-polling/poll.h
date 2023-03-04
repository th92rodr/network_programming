#ifndef POLL_H
#define POLL_H

#include <poll.h>

int create_file_descriptors_poll(int socket_fd, int *connections_num, struct pollfd **fds_poll);
int evaluate_event(int socket_fd, int *connections, struct pollfd **fds_poll);
static int add_connection_to_poll(int fd, int *connections, struct pollfd **fds_poll);
static int remove_connection_from_poll(int fd_index, int *connections, struct pollfd **fds_poll);
static void handle_new_connection(int socket_fd, int *connections, struct pollfd **fds_poll);
static void handle_same_connection(int request_socket_fd, int fd_index, int *connections, struct pollfd **fds_poll);

#endif
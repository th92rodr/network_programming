#ifndef SHARED_H
#define SHARED_H

int send_message(int socket_fd, char *message, int *total_bytes_sent);
int read_message(int socket_fd, char **message, int *total_bytes_read);

#endif
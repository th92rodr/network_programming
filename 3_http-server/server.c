/*
** http server
*/

#include "http.h"
#include "../1_server-client/server_functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int parse_request(struct request **req) {
    int payload_size = strlen((*req)->payload);
    char *payload = (char*) malloc((payload_size+1) * sizeof(char));
    strcpy(payload, (*req)->payload);
    payload[payload_size-1] = '\0';

    char *token;

    token = strtok(payload, " \t\r\n");
    if (token == NULL) {
        free(payload);
        fprintf(stderr, "Error parsing request\n");
        return -1;
    }

    (*req)->method = (char*) malloc((strlen(token) + 1) * sizeof(char));
    strcpy((*req)->method, token);
    (*req)->method[strlen(token)] = '\0';

    token = strtok(NULL, " \t");
    if (token == NULL) {
        free(payload);
        fprintf(stderr, "Error parsing request\n");
        return -1;
    }

    (*req)->url = (char*) malloc((strlen(token) + 1) * sizeof(char));
    strcpy((*req)->url, token);
    (*req)->url[strlen(token)] = '\0';

    char *query_str = strchr((*req)->url, '?');
    if (query_str != NULL) {
        *query_str++ = '\0';
    } else {
        query_str = "";
    }
    (*req)->query_str = query_str;

    token = strtok(NULL, " \t\r\n");
    if (token == NULL) {
        free(payload);
        fprintf(stderr, "Error parsing request\n");
        return -1;
    }

    (*req)->protocol = (char*) malloc((strlen(token) + 1) * sizeof(char));
    strcpy((*req)->protocol, token);
    (*req)->protocol[strlen(token)] = '\0';

    struct header_type *header_list = NULL;
    struct header_type *last = NULL;
    while (1) {
        char *key_token = strtok(NULL, "\r\n: \t");
        if (key_token == NULL) {
            break;
        }

        char *value_token = strtok(NULL, "\r\n");
        while(*value_token && *value_token == ' ') value_token++;

        struct header_type *header = (struct header_type*) malloc(sizeof(struct header_type));
        if (header == NULL) {
            free(payload);
            fprintf(stderr, "Error allocating headers struct\n");
            return -1;
        }
        memset(header, 0, sizeof(struct header_type));

        char *key = (char*) malloc((strlen(key_token) + 1) * sizeof(char));
        strcpy(key, key_token);
        key[strlen(key_token)] = '\0';

        char *value = (char*) malloc((strlen(value_token) + 1) * sizeof(char));
        strcpy(value, value_token);
        value[strlen(value_token)] = '\0';

        header->name = key;
        header->value = value;
        header->next = NULL;
        printf(" [H] %s : %s\n", header->name, header->value);

        if (header_list == NULL) {
            header_list = header;
        } else {
            last->next = header;
        }
        last = header;
    }
    (*req)->headers = header_list;

    free(payload);
    return 0;
}

int read_request(int request_socket_fd, char **request) {
    const int FLAGS = 0;
    const int MAX_BUFFER_SIZE = 1024;

    int buffer_size = MAX_BUFFER_SIZE;
    char *buffer = (char*) malloc(buffer_size * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Error allocating buffer\n");
        return -1;
    }
    memset(buffer, '\0', buffer_size * sizeof(char));

    int total_bytes_read = 0;
    while (1) {
        char *tmp_buffer = (char*) malloc(MAX_BUFFER_SIZE * sizeof(char));
        int bytes_read = recv(request_socket_fd, tmp_buffer, MAX_BUFFER_SIZE, FLAGS);
        if (bytes_read <= 0) {
            perror("Error receiving message");
            return -1;
        }

        if (total_bytes_read + bytes_read >= buffer_size) {
            buffer_size += bytes_read;
            buffer = realloc(buffer, buffer_size * sizeof(char));
            if (buffer == NULL) {
                fprintf(stderr, "Error reallocating buffer\n");
                return -1;
            }
        }

        memcpy(buffer + total_bytes_read, tmp_buffer, bytes_read);
        total_bytes_read += bytes_read;

        if (strstr(tmp_buffer, "\r\n\r\n")) {
            free(tmp_buffer);
            break;
        }
    }

    buffer_size = total_bytes_read + 1;
    buffer = realloc(buffer, buffer_size * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Error reallocating buffer\n");
        return -1;
    }
    buffer[buffer_size-1] = '\0';

    *request = buffer;

    return 0;
}

int handle_request(int request_socket_fd) {
    char *payload;
    if (read_request(request_socket_fd, &payload) == -1) {
        return -1;
    }
    printf("Request length: %ld\n", strlen(payload));
    printf("Request: %s\n", payload);
    printf("\n-------------\n");

    struct request *req = NULL;
    req = (struct request*) malloc(sizeof(struct request));
    if (req == NULL) {
        free(payload);
        fprintf(stderr, "Error allocating request struct\n");
        return -1;
    }
    memset(req, 0, sizeof(struct request));
    req->payload = payload;

    parse_request(&req);

    printf("\n-------------\n");
    printf("Payload: %s\n", req->payload);
    printf("Method: %s\n", req->method);
    printf("Url: %s\n", req->url);
    printf("Query: %s\n", req->query_str);
    printf("Protocol: %s\n", req->protocol);

    printf("Headers:\n");
    for (struct header_type *current = req->headers; current != NULL; current = current->next) {
        printf(" %s : %s\n", current->name, current->value);
    }

    free(req->payload);
    free(req->method);
    free(req->url);
    free(req->protocol);

    struct header_type *current = req->headers;
    while (current != NULL) {
        struct header_type *temp = current->next;
        free(current->name);
        free(current->value);
        free(current);
        current = temp;
    }

    free(req);

    return 0;
}

int get_home_page(char **body) {
    FILE *file;
    if ((file = fopen("./index.html", "r")) == NULL) {
        perror("Error opening file");
        return -1;
    }

    char c;
    char *buffer;
    int buffer_size = 0;

    while ((c = fgetc(file)) != EOF) {
        buffer_size++;

        if (buffer_size == 1) {
            buffer = (char*) malloc(buffer_size * sizeof(char));
            if (buffer == NULL) {
                fprintf(stderr, "Error allocating buffer\n");
                return -1;
            }

        } else {
            char *tmp_buffer = realloc(buffer, buffer_size * sizeof(char));
            if (tmp_buffer == NULL) {
                fprintf(stderr, "Error reallocating buffer\n");
                return -1;
            }
            buffer = tmp_buffer;
        }

        buffer[buffer_size-1] = c;
    }

    buffer_size++;
    buffer = realloc(buffer, buffer_size * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Error reallocating buffer\n");
        return -1;
    }
    buffer[buffer_size-1] = '\0';

    if (fclose(file) == EOF) {
        perror("Error closing file");
    }

    *body = buffer;

    return 0;
}

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

        close(request_socket_fd);
    }

    return 0;
}

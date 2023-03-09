#ifndef HTTP_H
#define HTTP_H

struct request {
    char *payload;

    char *method;
    char *url;
    char *protocol;
    char *query_str;
    char *body;

    struct header_type *headers;
};

struct header_type {
    char *name;
    char *value;
    struct header_type *next;
};

int handle_request(int request_socket_fd);
static int read_request(int request_socket_fd, char **request);
static int parse_request(struct request **req);
static char *get_header(struct header_type *headers, const char* name);

int build_response(int request_socket_fd);
static int get_home_page(char **body);

#endif
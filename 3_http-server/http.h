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

#endif
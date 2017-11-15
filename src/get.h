#ifndef GET_H
#define GET_H 1

#include <curl/curl.h>

struct response {
    char *type;
    char *content;
    int length;
};

struct response* new_response(void);
void free_response(struct response*);

typedef struct buffer_length_tracker {
    char *content;
    int length;
} buffer_t;

buffer_t* new_buffer(void);
void free_buffer(buffer_t*);
size_t append_buffer(char*, size_t, size_t, buffer_t*);

/* Abstraction for curl performance */
CURLcode get(struct response*, char*);

#endif /* GET_H */

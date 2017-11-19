#ifndef GET_H
#define GET_H

#include <curl/curl.h>

struct response {
    char *type;
    char *content;
    int length;
};

void free_response(struct response*);

typedef struct buffer_length_tracker {
    char *content;
    int length;
} buffer_t;

buffer_t* new_buffer(void);
void free_buffer(buffer_t*);
size_t append_buffer(char*, size_t, size_t, buffer_t*);

/* Abstraction for curl performance */
struct response* get_response(char*);

#endif /* GET_H */

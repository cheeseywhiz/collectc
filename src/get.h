#ifndef GET_H
#define GET_H

#include <stdlib.h>

struct response {
    char *type;
    char *content;
    size_t length;
    char *url;
};

void free_response(struct response *self);

/* Abstraction for curl performance */
struct response* get_response(char *url);
int get_download_response(struct response *self, char *path);
struct response* get_image(char *url);

#endif /* GET_H */

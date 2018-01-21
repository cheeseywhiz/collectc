#ifndef GET_H
#define GET_H

#include <stdlib.h>

#include "jsmnutils.h"

struct response {
    char *type;
    char *content;
    size_t length;
    char *url;
};

void get_free_response(struct response *self);

/* Abstraction for curl performance */
struct response* get_response(char *url);
int get_download_response(struct response *self, char *path);
struct response* get_image(char *url);
ju_json_t* get_json(char *url);
void get_free_json(ju_json_t *json);

#endif /* GET_H */

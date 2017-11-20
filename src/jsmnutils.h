#ifndef JSON_H
#define JSON_H

#include "jsmn.h"

typedef struct {
    char *json_str;
    jsmntok_t *tokens;
    int n_tokens;
} ju_json_t;

enum key_error {
    E_NOT_OBJECT = -1,
    E_NO_MATCH = -2
};

ju_json_t* ju_parse(char*);
void ju_free(ju_json_t*);

int ju_object_get(ju_json_t*, int, char*);

struct ju_array_iter {
    ju_json_t *json;
    int n_items;
    int array_i;
    int index;
};

struct ju_array_iter ju_init_array_iter(ju_json_t*, int);
int ju_array_next(struct ju_array_iter*);

struct ju_array_iter ju_init_url_iter(ju_json_t*);
char* ju_next_url(struct ju_array_iter*);

#endif

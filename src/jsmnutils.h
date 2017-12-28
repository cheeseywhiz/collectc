#ifndef JSON_H
#define JSON_H

#include "jsmn.h"
#include "random_popper.h"

typedef struct {
    char *json_str;
    jsmntok_t *tokens;
    int n_tokens;
} ju_json_t;

enum key_error {
    JU_ETYPE = -1,
    JU_ENO_MATCH = -2
};

ju_json_t* ju_parse(char *json_str);
void ju_free(ju_json_t *self);

int ju_object_get(ju_json_t *self, int object, char *key);

struct ju_array_iter {
    ju_json_t *json;
    int n_items;
    int array_i;
    int index;
    int size;
};

struct ju_array_iter* ju_array_init(ju_json_t *self, int array_i);
int ju_array_next(struct ju_array_iter *self);

rp_t* ju_array_rp(ju_json_t *self, int array_i);

#endif

#ifndef JSON_H
#define JSON_H

#include "jsmn.h"
#include "rand.h"

typedef struct {
    char *json_str;
    jsmntok_t *tokens;
    int n_tokens;
} ju_json_t;

enum key_error {
    JU_ETYPE = -1,
    JU_ENO_MATCH = -2
};

ju_json_t* ju_parse(char*);
void ju_free(ju_json_t*);

int ju_object_get(ju_json_t*, int, char*);

struct ju_array_iter {
    ju_json_t *json;
    int n_items;
    int array_i;
    int index;
    int size;
};

struct ju_array_iter* ju_array_init(ju_json_t*, int);
int ju_array_next(struct ju_array_iter*);

struct ju_random_iter {
    ju_json_t *json;
    int *list;
    int_list *indices;
    int index;
};

struct ju_random_iter* ju_random_init(ju_json_t*, int);
void ju_random_free(struct ju_random_iter*);
int ju_random_next(struct ju_random_iter*);

#endif

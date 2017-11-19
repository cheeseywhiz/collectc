#include <string.h>

#define JSMN_PARENT_LINKS
#include "jsmn.h"
#include "jsmnutils.h"
#include "reg.h"

#define MAX_TOKENS 32768

void ju_parse(ju_json_t *self, char *json_str) {
    jsmn_parser parser;
    jsmn_init(&parser);

    int n_tokens = jsmn_parse(&parser, json_str, strlen(json_str), NULL, 0);

    if (n_tokens < 0) {
        n_tokens = MAX_TOKENS;
    };

    jsmn_init(&parser);
    jsmntok_t tokens[n_tokens];
    n_tokens = jsmn_parse(&parser, json_str, strlen(json_str), tokens, n_tokens);

    if (n_tokens < 0) {
        n_tokens = MAX_TOKENS;
    };

    self->json_str = json_str;
    self->tokens = tokens;
    self->n_tokens = n_tokens;
};

int ju_key_search(ju_json_t *self, int object, char *key) {
    if (self->tokens[object].type != JSMN_OBJECT) {
        return E_NOT_OBJECT;
    };

    for (int i = object; i < self->n_tokens; i++) {
        int start = self->tokens[i].start;
        int end = self->tokens[i].end;
        int length = end - start;
        char *token = self->json_str + start;

        if (self->tokens[i].parent == object && length == strlen(key) && regex_starts_with(token, key)) {
            i++;
            return i;
        };
    };

    return E_NO_MATCH;
};

struct ju_array_iter ju_init_array_iter(ju_json_t *self, int array_i) {
    struct ju_array_iter iter;
    iter.json = self;
    iter.n_items = 0;
    iter.index = array_i;
    iter.array_i = array_i;
    return iter;
};

int ju_array_next(struct ju_array_iter *self) {
    self->n_items++;

    if (!(self->n_items <= self->json->tokens[self->array_i].size)) {
        return -1;
    };

    for (self->index++; self->index < self->json->n_tokens; self->index++) {
        if (self->json->tokens[self->index].parent == self->array_i) {
            return self->index;
        };
    };

    return -1;
};


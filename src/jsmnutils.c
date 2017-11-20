#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "jsmn.h"
#include "jsmnutils.h"
#include "reg.h"

#define JU_MAX_TOKENS 32768

ju_json_t* ju_parse(char *json_str) {
    size_t self_size = sizeof(ju_json_t);
    ju_json_t *self = malloc(self_size);

    if (self) {
        bzero(self, self_size);
    } else {
        return NULL;
    };

    jsmn_parser parser;
    jsmn_init(&parser);

    self->json_str = json_str;
    self->n_tokens = jsmn_parse(&parser, json_str, strlen(json_str), NULL, 0);

    if (0 > self->n_tokens) {
        free(self);
        return NULL;
    };

    size_t tokens_size = self->n_tokens * sizeof(jsmntok_t);
    jsmn_init(&parser);
    jsmntok_t *tokens = malloc(tokens_size);

    if (tokens) {
        bzero(tokens, tokens_size);
        self->tokens = tokens;
    } else {
        free(self);
        return NULL;
    };

    if (0 < jsmn_parse(&parser, json_str, strlen(json_str), tokens, self->n_tokens)) {
        return self;
    } else {
        ju_free(self);  /* self->tokens and self */
        return NULL;
    };
};

void ju_free(ju_json_t *self) {
    free(self->tokens);
    free(self);
};

int ju_object_get(ju_json_t *self, int object, char *key) {
    struct ju_array_iter *iter = ju_init_array_iter(self, object);
    
    if (!iter) {
        return JU_ETYPE;
    };
    
    int i;
    
    for (i = ju_array_next(iter); i > 0; i = ju_array_next(iter)) {
        jsmntok_t token = self->tokens[i];
        char *tok_str = regex_str_slice(self->json_str, token.start, token.end);

        if (!tok_str) {
            continue;
        };

        int str_eq = (strcmp(tok_str, key) == 0);
        free(tok_str);

        if (str_eq) {
            free(iter);
            i++;
            return i;
        };
    };

    free(iter);
    return JU_ENO_MATCH;
};

struct ju_array_iter* ju_init_array_iter(ju_json_t *self, int array_i) {
    int tok_type = self->tokens[array_i].type;

    if (tok_type != JSMN_ARRAY && tok_type != JSMN_OBJECT) {
        return NULL;
    };
    
    size_t iter_size = sizeof(struct ju_array_iter);
    struct ju_array_iter *iter = malloc(iter_size);
    
    if (iter) {
        bzero(iter, iter_size);
    } else {
        return NULL;
    };

    iter->json = self;
    iter->n_items = 0;
    iter->index = array_i;
    iter->array_i = array_i;
    return iter;
};

int ju_array_next(struct ju_array_iter *self) {
    self->n_items++;

    if (self->n_items > self->json->tokens[self->array_i].size) {
        return -1;
    };

    for (self->index++; self->index < self->json->n_tokens; self->index++) {
        if (self->json->tokens[self->index].parent == self->array_i) {
            return self->index;
        };
    };

    return -1;
};

struct ju_array_iter* ju_init_url_iter(ju_json_t *self) {
    int data_obj_i = ju_object_get(self, 0, "data");
    int posts_arr_i = ju_object_get(self, data_obj_i, "children");
    return ju_init_array_iter(self, posts_arr_i);
};

char* ju_next_url(struct ju_array_iter *self) {
    int i = ju_array_next(self);

    if (i < 0) {
        return NULL;
    };

    int sub_data_i = ju_object_get(self->json, i, "data");
    int url_i = ju_object_get(self->json, sub_data_i, "url");
    int start = self->json->tokens[url_i].start;
    int end = self->json->tokens[url_i].end;
    return regex_str_slice(self->json->json_str, start, end);
};

#include <stdlib.h>
#include <string.h>

#include "jsmn.h"
#include "jsmnutils.h"
#include "reg.h"

ju_json_t* ju_parse(char *json_str) {
    ju_json_t *self = malloc(sizeof(ju_json_t));

    if (!self) {
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

    jsmn_init(&parser);
    jsmntok_t *tokens = calloc(self->n_tokens, sizeof(jsmntok_t));

    if (tokens) {
        self->tokens = tokens;
    } else {
        free(self);
        return NULL;
    };

    if (0 < jsmn_parse(&parser, self->json_str, strlen(json_str), self->tokens, self->n_tokens)) {
        return self;
    } else {
        ju_free(self);  /* self->tokens and self */
        return NULL;
    };
}

void ju_free(ju_json_t *self) {
    free(self->tokens);
    free(self);
}

int ju_object_get(ju_json_t *self, int object, char *key) {
    struct ju_array_iter *iter = ju_array_init(self, object);
    
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
}

struct ju_array_iter* ju_array_init(ju_json_t *self, int array_i) {
    int tok_type = self->tokens[array_i].type;

    if (tok_type != JSMN_ARRAY && tok_type != JSMN_OBJECT) {
        return NULL;
    };

    struct ju_array_iter *iter = malloc(sizeof(struct ju_array_iter));
    
    if (!iter) {
        return NULL;
    };

    iter->json = self;
    iter->n_items = 0;
    iter->index = array_i;
    iter->array_i = array_i;
    iter->size = self->tokens[array_i].size;
    return iter;
}

int ju_array_next(struct ju_array_iter *self) {
    if (self->n_items < 0) {
        return -1;
    };

    self->n_items++;

    if (self->n_items > self->size) {
        return -1;
    };

    for (self->index++; self->index < self->json->n_tokens; self->index++) {
        if (self->json->tokens[self->index].parent == self->array_i) {
            return self->index;
        };
    };

    return -1;
}

struct ju_array_iter* ju_url_init(ju_json_t *self) {
    int data_obj_i = ju_object_get(self, 0, "data");
    int posts_arr_i = ju_object_get(self, data_obj_i, "children");

    if (posts_arr_i < 1) {
        return NULL;
    };

    return ju_array_init(self, posts_arr_i);
}

static char* ju_url_from_post_object(ju_json_t *json, int post_i) {
    if (post_i < 0) {
        return NULL;
    };

    int sub_data_i = ju_object_get(json, post_i, "data");
    int url_i = ju_object_get(json, sub_data_i, "url");

    if (url_i < 0) {
        return NULL;
    };

    int start = json->tokens[url_i].start;
    int end = json->tokens[url_i].end;
    return regex_str_slice(json->json_str, start, end);
}

char* ju_url_next(struct ju_array_iter *self) {
    return ju_url_from_post_object(self->json, ju_array_next(self));
}

struct ju_random_iter* ju_random_init(ju_json_t *self, int array_i) {
    struct ju_random_iter *iter = malloc(sizeof(struct ju_random_iter));

    if (!iter) {
        return NULL;
    };

    struct ju_array_iter *array_iter = ju_array_init(self, array_i);

    if (!array_iter) {
        return NULL;
    };

    iter->list = malloc(sizeof(int) * array_iter->size);

    if (!iter->list) {
        return NULL;
    };

    int json_i;
    int length = 0;

    for (json_i = ju_array_next(array_iter); json_i > 0; json_i = ju_array_next(array_iter)) {
        iter->list[length++] = json_i;
    };

    iter->json = self;
    iter->indices = int_list_random_order(length);
    iter->index = 0;
    free(array_iter);
    return iter;
}

void ju_random_free(struct ju_random_iter *self) {
    free_int_list(self->indices);
    free(self->list);
    free(self);
}

#include <stdio.h>

int ju_random_next(struct ju_random_iter *self) {
    self->index++;

    if (self->index >= self->indices->length) {
        return -1;
    };

    return self->list[self->indices->items[self->index]];
}

struct ju_random_iter* ju_random_url_init(ju_json_t *self) {
    int data_obj_i = ju_object_get(self, 0, "data");
    int posts_arr_i = ju_object_get(self, data_obj_i, "children");

    if (posts_arr_i < 1) {
        return NULL;
    };

    return ju_random_init(self, posts_arr_i);
}

char* ju_random_url_next(struct ju_random_iter *self) {
    return ju_url_from_post_object(self->json, ju_random_next(self));
}

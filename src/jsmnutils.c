#include <stdlib.h>
#include <string.h>

#include "jsmn.h"
#include "jsmnutils.h"
#include "reg.h"
#include "random_popper.h"

ju_json_t* ju_parse(char *json_str) {
    ju_json_t *self = malloc(sizeof(ju_json_t));

    if (!self) {
        return NULL;
    }

    jsmn_parser parser;
    jsmn_init(&parser);

    self->json_str = json_str;
    self->n_tokens = jsmn_parse(&parser, json_str, strlen(json_str), NULL, 0);

    if (0 > self->n_tokens) {
        free(self);
        return NULL;
    }

    jsmn_init(&parser);
    jsmntok_t *tokens = calloc(self->n_tokens, sizeof(jsmntok_t));

    if (tokens) {
        self->tokens = tokens;
    } else {
        free(self);
        return NULL;
    }

    if (0 < jsmn_parse(&parser, self->json_str, strlen(json_str), self->tokens, self->n_tokens)) {
        return self;
    } else {
        ju_free(self);  /* self->tokens and self */
        return NULL;
    }
}

void ju_free(ju_json_t *self) {
    free(self->tokens);
    free(self);
}

int ju_object_get(ju_json_t *self, int object, char *key) {
    struct ju_array_iter *iter = ju_array_init(self, object);
    
    if (!iter) {
        return JU_ETYPE;
    }
    
    int i;

    for (i = ju_array_next(iter); i > 0; i = ju_array_next(iter)) {
        jsmntok_t token = self->tokens[i];
        char *tok_str = regex_str_slice(self->json_str, token.start, token.end);

        if (!tok_str) {
            continue;
        }

        int str_eq = (strcmp(tok_str, key) == 0);
        free(tok_str);

        if (str_eq) {
            free(iter);
            i++;
            return i;
        }
    }

    free(iter);
    return JU_ENO_MATCH;
}

struct ju_array_iter* ju_array_init(ju_json_t *self, int array_i) {
    int tok_type = self->tokens[array_i].type;

    if (tok_type != JSMN_ARRAY && tok_type != JSMN_OBJECT) {
        return NULL;
    }

    struct ju_array_iter *iter = malloc(sizeof(struct ju_array_iter));
    
    if (!iter) {
        return NULL;
    }

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
    }

    self->n_items++;

    if (self->n_items > self->size) {
        return -1;
    }

    for (self->index++; self->index < self->json->n_tokens; self->index++) {
        if (self->json->tokens[self->index].parent == self->array_i) {
            return self->index;
        }
    }

    return -1;
}

rp_t* ju_array_rp(ju_json_t *self, int array_i) {
    struct ju_array_iter *array_iter = ju_array_init(self, array_i);

    if (!array_iter) {
        return NULL;
    }

    int json_i;
    rp_t *popper = NULL;

    for (json_i = ju_array_next(array_iter); json_i > 0; json_i = ju_array_next(array_iter)) {
        if (!rp_append(&popper, json_i)) {
            break;
        }
    }

    free(array_iter);
    return popper;
}

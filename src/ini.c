#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "jsmn.h"
#include "ini.h"
#include "jsmnutils.h"
#include "path.h"

#define __JSMN_TOK(type_, start_, end_, size_, parent_) \
    (jsmntok_t) { \
        .type = type_, \
        .start = start_, \
        .end = end_, \
        .size = size_, \
        .parent = parent_, \
    }

static int parse_append(jsmntok_t *tokens, unsigned int num_tokens, unsigned int i, jsmntok_t token) {
    if (!tokens) return 0;

    if (i >= num_tokens) {
        EXCEPTION("Insufficient memory");
        return JSMN_ERROR_NOMEM;
    }

    tokens[i] = token;
    return 0;
}

#define __APPEND_TOK(type, start, end, size, parent) \
    ret = parse_append(tokens, num_tokens, counter++, __JSMN_TOK(type, start, end, size, parent)); \
    if (ret < 0) return ret

#define __APPEND() __APPEND_TOK(type, start, end, end - start, parent)

static int ini_parse_impl(char *is, size_t len, jsmntok_t *tokens, unsigned int num_tokens) {
    int ret;
    int counter = 0;
    unsigned int position = 0;
    unsigned int super_object = 0, super_key = 0;
    jsmntype_t type;
    int start, end, parent;

    /* The entire string is an object */
    type = JSMN_OBJECT;
    start = 0;
    end = len;
    parent = 0;
    __APPEND();

    for (; position < len && is[position] != '\0'; position++) {
        switch (is[position]) {
            case ';': case '#': /* comment */
                while (is[++position] != '\n');
                break;
            case '\n': /* empty line */
                break;
            case '[':
                /* new key to object */
                type = JSMN_STRING;
                start = ++position;
                while (is[++position] != ']');
                end = position;
                parent = 0;
                __APPEND();
                /* new object */
                type = JSMN_OBJECT;
                start = ++position;
                do { while (is[position++] != '\n'); } while (is[position] != '[' && is[position] != '\0');
                end = position;
                position = start;
                parent = counter - 1;
                super_object = counter;
                __APPEND();
                break;
            case '=': /* value */
                type = JSMN_STRING;
                start = ++position;
                while (is[++position] != '\n');
                end = position;
                parent = super_key;
                __APPEND();
                break;
            default: /* key */
                type = JSMN_STRING;
                start = position;
                while (is[++position] != '=');
                end = position--;
                parent = super_object;
                super_key = counter;
                __APPEND();
                break;
        }
    }

    return counter;
}

ju_json_t* ini_parse(char *ini_str) {
    ju_json_t *self = malloc(sizeof(ju_json_t));

    if (!self) {
        LOG_ERRNO();
        return NULL;
    }

    self->json_str = ini_str;
    self->n_tokens = ini_parse_impl(ini_str, strlen(ini_str), NULL, 0);

    if (0 > self->n_tokens) {
        free(self);
        return NULL;
    }

    jsmntok_t *tokens = calloc(self->n_tokens, sizeof(jsmntok_t));

    if (tokens) {
        self->tokens = tokens;
    } else {
        LOG_ERRNO();
        free(self);
        return NULL;
    }

    if (0 < ini_parse_impl(ini_str, strlen(ini_str), self->tokens, self->n_tokens)) {
        return self;
    } else {
        ju_free(self);  /* self->tokens and self */
        return NULL;
    }
}

ju_json_t* ini_parse_path(char *path) {
    char *contents = path_contents(path);
    if (!contents) return NULL;

    ju_json_t *json = ini_parse(contents);

    if (!json) {
        free(contents);
        return NULL;
    }

    return json;
}

void ini_free_path(ju_json_t *json) {
    free(json->json_str);
    ju_free(json);
}

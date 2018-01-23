#ifndef GET_H
#define GET_H

#include <curl/curl.h>
#include <stdlib.h>

#include "jsmnutils.h"

struct var_len_buffer {
    char *content;
    size_t length;
    size_t size;
};

struct get_handle {
    CURL *handle;
    struct var_len_buffer hd_buf;
    struct var_len_buffer ct_buf;
};

int get_init_handle(struct get_handle *self);
void get_cleanup_handle(struct get_handle *self);
void get_reset_handle(struct get_handle *self);

struct response {
    char *type;
    char *content;
    size_t length;
    char *url;
};

int get_perform(struct get_handle *self, struct response *re, char *url);
void get_free_response(struct response *self);

/* Abstraction for curl performance */
int get_response(struct response *re, char *url);
int get_download_response(struct response *self, char *path);
int get_image(struct response *re, char *url);
ju_json_t* get_json(struct get_handle *handle, char *url);
void get_free_json(ju_json_t *json);

#endif /* GET_H */

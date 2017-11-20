#include <curl/curl.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "get.h"
#include "reg.h"

void free_response(struct response *self) {
    free(self->type);
    free(self->content);
    free(self);
};

typedef struct {
    char *content;
    size_t length;
} buffer_t;

static buffer_t* new_buffer(void) {
    buffer_t *self = malloc(sizeof(buffer_t));

    if (!self) {
        fprintf(stderr, "malloc buffer_t\n");
        return NULL;
    };

    self->content = malloc(1);

    if (self->content) {
        *self->content = 0;
    } else {
        fprintf(stderr, "malloc buffer_t->content\n");
        return NULL;
    };

    self->length = 0;
    return self;
};

static void free_buffer(buffer_t *self) {
    free(self->content);
    free(self);
};

static size_t append_buffer(char *buffer, size_t len, size_t n_items, buffer_t *self) {
    size_t n_chars = len * n_items;
    char *p = realloc(self->content, self->length + n_chars + 3);

    if (p) {
        self->content = p;
        strcat(self->content, buffer);
        self->length += n_chars;
        return n_chars;
    } else {
        return 0;
    };

};

struct response* get_response(char *url) {
    struct response *re = malloc(sizeof(struct response));

    if (!re) {
        fprintf(stderr, "malloc struct response*\n");
        return NULL;
    };

    CURLcode res = 0;
    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "curl init failed\n");
        free_response(re);
        return NULL;
    };

    char *ua_prefix = "collect/";
    char user_agent[strlen(ua_prefix) + strlen(VERSION)];
    strcpy(user_agent, ua_prefix);
    strcat(user_agent, VERSION);

    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
    curl_easy_setopt(curl, CURLOPT_URL, url);

    buffer_t *hd_buf = new_buffer();
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, append_buffer);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, hd_buf);
    
    buffer_t *ct_buf = new_buffer();
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, append_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, ct_buf);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform failed (%s)\n", curl_easy_strerror(res));
        free_response(re);
        return NULL;
    };

    re->content = malloc(ct_buf->length + 3);
    strcpy(re->content, ct_buf->content);

    re->length = ct_buf->length;

    char *pattern = "content-type:[ ]*([^\r\n]*)";
    char *match = regex_match_one_subexpr(pattern, hd_buf->content, REG_EXTENDED | REG_ICASE);
    re->type = malloc(strlen(match) + 3);
    strcpy(re->type, match);

    free(match);
    free_buffer(hd_buf);
    free_buffer(ct_buf);

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return re;
};

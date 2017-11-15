#include <curl/curl.h>
#include <regex.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "get.h"
#include "reg.h"

struct response* new_response(void) {
    return malloc(sizeof(struct response));
};

void free_response(struct response *self) {
    free(self->type);
    free(self);
};

buffer_t* new_buffer(void) {
    buffer_t *self = malloc(sizeof(buffer_t));
    self->content = malloc(0);
    self->length = 0;
    return self;
};

void free_buffer(buffer_t *self) {
    free(self->content);
    free(self);
};

size_t append_buffer(char *buffer, size_t len, size_t n_items, buffer_t *self) {
    size_t n_chars = len * n_items;
    self->length += n_chars;
    self->content = realloc(self->content, self->length + 1);
    strcat(self->content, buffer);
    return n_chars;
};

CURLcode get(struct response *re, char *url) {
    CURLcode res = 0;
    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "curl init failed\n");
        return 1;
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
    };

    re->content = ct_buf->content;
    re->length = ct_buf->length;

    char *pattern = "content-type:[ ]*([^\r\n]*)";
    re->type = regex_match_one_subexpr(pattern, hd_buf->content, REG_EXTENDED | REG_ICASE);

    free_buffer(hd_buf);
    free_buffer(ct_buf);

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return res;
};

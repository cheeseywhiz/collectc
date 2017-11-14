#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include "get.h"
#include "config.h"

buffer_t* new_buffer(void) {
    buffer_t *buf = malloc(sizeof(buffer_t));
    buf->content = malloc(0);
    buf->length = 0;
    return buf;
};

void free_buffer(buffer_t *buf) {
    free(buf->content);
    free(buf);
};

size_t append_buffer(char *buffer, size_t len, size_t n_items, buffer_t *buf) {
    size_t n_chars = len * n_items;
    buf->length += n_chars;
    buf->content = realloc(buf->content, buf->length + 1);
    strcat(buf->content, buffer);
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

    free_buffer(hd_buf);
    free_buffer(ct_buf);

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return res;
};

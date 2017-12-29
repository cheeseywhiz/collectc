#include <curl/curl.h>
#include <regex.h>
#include <string.h>

#include "config.h"
#include "get.h"
#include "reg.h"

#define UA_PREFIX "collectc"

void free_response(struct response *self) {
    free(self->type);

    if (self->content) {
        free(self->content);
    }

    free(self);
}

typedef struct {
    char *content;
    size_t length;
} buffer_t;

static buffer_t* new_buffer(void) {
    buffer_t *self = malloc(sizeof(buffer_t));

    if (!self) {
        return NULL;
    };

    self->content = NULL;
    self->length = 0;
    return self;
}

static size_t append_buffer(char *buffer, size_t size, size_t n_items, buffer_t *self) {
    size_t buffer_len = size * n_items;
    char *ptr = realloc(self->content, self->length + buffer_len + 1);

    if (ptr) {
        self->content = ptr;
    } else {
        return 0;
    };

    memcpy(self->content + self->length, buffer, buffer_len);
    self->length += buffer_len;
    self->content[self->length] = '\0';

    return buffer_len;
}

struct response* get_response(char *url) {
    int exit = 0;
    CURLcode res = 0;
    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        exit = 1;
        goto cleanup1;
    } else {
        char user_agent[strlen(UA_PREFIX) + 1 + strlen(VERSION) + 1];
        sprintf(user_agent, "%s/%s", UA_PREFIX, VERSION);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
    };

    curl_easy_setopt(curl, CURLOPT_URL, url);

    buffer_t *hd_buf = new_buffer();

    if (!hd_buf) {
        exit = 1;
        goto cleanup1;
    };

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, append_buffer);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, hd_buf);
    
    buffer_t *ct_buf = new_buffer();

    if (!ct_buf) {
        exit = 1;
        free(hd_buf);
        goto cleanup1;
    };

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, append_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, ct_buf);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform failed (%s)\n", curl_easy_strerror(res));
        exit = 1;
        goto cleanup2;
    };

    struct response *re = malloc(sizeof(struct response));

    if (!re) {
        exit = 1;
        goto cleanup2;
    };

    char *pattern = "content-type:[ ]*([^\r\n]*)";
    char *match = regex_match_one_subexpr(pattern, hd_buf->content, REG_EXTENDED | REG_ICASE);

    if (!match) {
        match = calloc(1, 1);

        if (!match) {
            exit = 1;
            free(re);
            goto cleanup2;
        };
    };

    re->type = match;
    re->content = ct_buf->content;
    re->length = ct_buf->length;
    re->url = url;

    if (hd_buf->content) {
        free(hd_buf->content);
    }

cleanup2:
    free(hd_buf);
    free(ct_buf);

cleanup1:
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (!exit) {
        return re;
    } else {
        return NULL;
    };
}

static int verify_image(struct response *re) {
    char *error_msg = "";

    if (regex_contains(re->url, "removed")) {
        error_msg = "Appears to be removed";
    }

    if (!regex_contains(re->type, "image")) {
        error_msg = "Not an image";
    }

    if (regex_contains(re->type, "gif")) {
        error_msg = "Is a .gif";
    }

    if (strlen(error_msg)) {
        return 0;
    } else {
        return 1;
    }
}

struct response* get_image(char *url) {
    struct response *re = get_response(url);

    if (!re) {
        return NULL;
    } else if (verify_image(re)) {
        return re;
    } else {
        return NULL;
    }
}

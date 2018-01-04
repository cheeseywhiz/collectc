#include <curl/curl.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "log.h"
#include "config.h"
#include "get.h"
#include "reg.h"
#include "path.h"

#define UA_PREFIX "collectc"

void free_response(struct response *self) {
    free(self->type);

    if (self->content) {
        free(self->content);
    }

    free(self);
}

struct var_len_buffer {
    char *content;
    size_t length;
};

#define __VAR_LEN_BUFFER(content_, length_) \
    (struct var_len_buffer) { \
        .content = content_, \
        .length = length_, \
    }

#define __NEW_BUFFER() __VAR_LEN_BUFFER(NULL, 0)

static size_t append_buffer(char *buffer, size_t size, size_t n_items, struct var_len_buffer *self) {
    size_t buffer_len = size * n_items;
    char *ptr = realloc(self->content, self->length + buffer_len + 1);

    if (ptr) {
        self->content = ptr;
    } else {
        LOG_ERRNO();
        return 0;
    };

    memcpy(self->content + self->length, buffer, buffer_len);
    self->length += buffer_len;
    self->content[self->length] = '\0';
    return buffer_len;
}

struct response* get_response(char *url) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if (!curl) {
        curl_global_cleanup();
        return NULL;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    char user_agent[strlen(UA_PREFIX) + 1 + strlen(VERSION) + 1];
    sprintf(user_agent, "%s/%s", UA_PREFIX, VERSION);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);

    struct var_len_buffer hd_buf = __NEW_BUFFER();
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, append_buffer);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &hd_buf);
    
    struct var_len_buffer ct_buf = __NEW_BUFFER();
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, append_buffer);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ct_buf);

    struct response *self = NULL;
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        ERROR("curl_easy_perform failed (%s)", curl_easy_strerror(res));
        goto cleanup;
    }

    self = malloc(sizeof(struct response));

    if (!self) {
        LOG_ERRNO();
        goto cleanup;
    }

    char *pattern = "content-type:[ ]*([^\r\n]*)";
    self->type = regex_match_one_subexpr(pattern, hd_buf.content, REG_ICASE);

    if (!self->type) {
        self->type = calloc(1, 1);

        if (!self->type) {
            LOG_ERRNO();
            free(self);
            self = NULL;
            goto cleanup;
        }
    }

    self->content = ct_buf.content;
    self->length = ct_buf.length;
    self->url = url;

    if (hd_buf.content) {
        free(hd_buf.content);
    }

cleanup:
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return self;
}

int get_download_response(struct response *self, char *path) {
    int status = 0;
    int file = path_open_write(path);

    if (file < 0) {
        return 1;
    } else if (write(file, self->content, self->length) < 0) {
        status = 1;
    }

    close(file);
    return status;
}

static int verify_image(struct response *self) {
    char *error_msg = "";

    if (regex_contains(self->url, "removed")) {
        error_msg = "Appears to be removed";
    }

    if (!regex_contains(self->type, "image")) {
        error_msg = "Not an image";
    }

    if (regex_contains(self->type, "gif")) {
        error_msg = "Is a .gif";
    }

    if (strlen(error_msg)) {
        DEBUG("%s (%s) (%s)", error_msg, self->url, self->type);
        return 0;
    } else {
        return 1;
    }
}

struct response* get_image(char *url) {
    struct response *self = get_response(url);

    if (!self) {
        return NULL;
    } else if (verify_image(self)) {
        return self;
    } else {
        free_response(self);
        return NULL;
    }
}

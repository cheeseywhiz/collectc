#include <curl/curl.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "log.h"
#include "get.h"
#include "reg.h"
#include "path.h"
#include "jsmnutils.h"

#define UA_PREFIX "collectc"

#define __VAR_LEN_BUFFER(content_, length_) \
    (struct var_len_buffer) { \
        .content = content_, \
        .length = length_, \
        .size = 0, \
    }

#define __NEW_BUFFER() __VAR_LEN_BUFFER(NULL, 0)

static char* buffer_realloc(struct var_len_buffer *self, size_t length) {
    size_t size = self->size;

    if (length <= size) {
        return self->content;
    } else if (!size) {
        size = 1 << 1;
    }

    do {
        size = (size >> 1) + size;
    } while (length > size);

    char *ptr = realloc(self->content, size + 1);

    if (ptr) {
        self->content = ptr;
    } else {
        LOG_ERRNO();
        return NULL;
    }

    self->size = size;
    return self->content;
}

static size_t append_buffer(char *buffer, size_t size, size_t n_items, struct var_len_buffer *self) {
    size_t buffer_len = size * n_items;

    if (!buffer_realloc(self, self->length + buffer_len)) {
        return 0;
    }

    memcpy(self->content + self->length, buffer, buffer_len);
    self->length += buffer_len;
    self->content[self->length] = '\0';
    return buffer_len;
}

int get_init_handle(struct get_handle *self) {
    self->handle = curl_easy_init();

    if (!self->handle) {
        EXCEPTION("curl_easy_init() failed");
        return 1;
    }

    char *fmt = "%s/%s by u/cheeseywhiz";
    size_t ua_len = strlen(fmt) - 4 + strlen(UA_PREFIX) + strlen(COLLECT_VERSION);
    char user_agent[ua_len + 1];
    sprintf(user_agent, fmt, UA_PREFIX, COLLECT_VERSION);
    curl_easy_setopt(self->handle, CURLOPT_USERAGENT, user_agent);
    curl_easy_setopt(self->handle, CURLOPT_HEADERFUNCTION, append_buffer);
    curl_easy_setopt(self->handle, CURLOPT_WRITEFUNCTION, append_buffer);
    return 0;
}

void get_cleanup_handle(struct get_handle *self) {
    curl_easy_cleanup(self->handle);
}

void get_reset_handle(struct get_handle *self) {
    self->hd_buf = __NEW_BUFFER();
    curl_easy_setopt(self->handle, CURLOPT_HEADERDATA, &self->hd_buf);

    self->ct_buf = __NEW_BUFFER();
    curl_easy_setopt(self->handle, CURLOPT_WRITEDATA, &self->ct_buf);
}

int get_perform(struct get_handle *self, struct response *re, char *url) {
    get_reset_handle(self);
    curl_easy_setopt(self->handle, CURLOPT_URL, url);
    CURLcode res = curl_easy_perform(self->handle);

    if (res != CURLE_OK) {
        EXCEPTION("curl_easy_perform failed (%s)", curl_easy_strerror(res));
        return 1;
    }

    if (self->hd_buf.content) {
        char *pattern = "content-type:[ ]*([^\r\n]*)";
        re->type = regex_match_one_subexpr(pattern, self->hd_buf.content, REG_ICASE);
        free(self->hd_buf.content);
        if (!re->type) return 2;
    } else {
        re->type = NULL;
    }

    re->content = self->ct_buf.content;
    re->length = self->ct_buf.length;
    re->url = url;
    return 0;
}

void get_free_response(struct response *self) {
    free(self->type);
    free(self->content);
}

int get_response(struct response *re, char *url) {
    struct get_handle handle;
    int ret;

    ret = get_init_handle(&handle);
    if (ret) return ret;

    ret = get_perform(&handle, re, url);
    curl_easy_cleanup(handle.handle);
    return ret;
}

int get_download_response(struct response *self, char *path) {
    int status = 0;
    int file = path_open_write(path);

    if (file < 0) {
        LOG_ERRNO();
        return 1;
    } else if (write(file, self->content, self->length) < 0) {
        LOG_ERRNO();
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
        return 1;
    } else {
        return 0;
    }
}

int get_image(struct response *re, char *url) {
    int ret;

    ret = get_response(re, url);
    if (ret) return ret;

    ret = verify_image(re);
    if (ret) get_free_response(re);

    return ret;
}

ju_json_t* get_json(struct get_handle *handle, char *url) {
    struct response re;

    if (get_perform(handle, &re, url)) {
        return NULL;
    } else if (!re.content || !regex_contains(re.type, "application/json")) {
        EXCEPTION("content or type check failed");
        get_free_response(&re);
        return NULL;
    }

    ju_json_t *json = ju_parse(re.content);

    if (!json) {
        get_free_response(&re);
        return NULL;
    }

    free(re.type);
    return json;
}

void get_free_json(ju_json_t *self) {
    free(self->json_str);
    ju_free(self);
}

#include <unistd.h>
#include <errno.h>

#include "raw.h"
#include "path.h"
#include "get.h"
#include "reg.h"

struct raw_post* raw_new_post(char *parent_path, ju_json_t *json, int index) {
    struct raw_post *self = malloc(sizeof(struct raw_post));

    if (!self) {
        return NULL;
    }

    self->json = json;
    self->index = index;
    char *url = raw_post_data_get(self, "url");

    if (!url) {
        free(self);
        return NULL;
    }

    self->url = url;
    char *path = path_url_fname(parent_path, self->url);

    if (!path) {
        free(self->url);
        free(self);
        return NULL;
    }

    self->path = path;
    return self;
}

void raw_free_post(struct raw_post *self) {
    free(self->path);
    free(self->url);
    free(self);
}


char* raw_post_data_get(struct raw_post *self, char *key) {
    int index = ju_object_get(self->json, self->index, key);

    if (index < 0) {
        return calloc(1, 1);
    }

    jsmntok_t tok = self->json->tokens[index];
    return regex_str_slice(self->json->json_str, tok.start, tok.end);
}

int raw_post_download(struct raw_post *self) {
    struct response *re = get_image(self->url);

    if (!re) {
        return -1;
    }

    return get_download_response(re, self->path);
}

static struct raw_base_listing* new_base_listing(ju_json_t *json) {
    struct raw_base_listing *self = malloc(sizeof(struct raw_base_listing));

    if (!self) {
        return NULL;
    }

    self->json = json;
    int data_obj_i = ju_object_get(self->json, 0, "data");
    int posts_arr_i = ju_object_get(self->json, data_obj_i, "children");

    if (posts_arr_i < 1) {
        free(self);
        return NULL;
    }

    self->popper = ju_array_rp(self->json, posts_arr_i);

    if (!self->popper) {
        free(self);
        return NULL;
    }

    return self;
}

static void free_base_listing(struct raw_base_listing *self) {
    rp_deep_free(&self->popper);
    ju_free(self->json);
    free(self);
}

static int base_listing_next(struct raw_base_listing *self) {
    int *iter_next = rp_pop_random(&self->popper);

    if (!iter_next) {
        return -1;
    }

    int next_item = ju_object_get(self->json, *iter_next, "data");
    free(iter_next);
    return next_item;
}

raw_listing* raw_listing_data(char *path, ju_json_t *json) {
    raw_listing *self = malloc(sizeof(raw_listing));

    if (!self) {
        return NULL;
    }

    self->path = path_norm(path);

    if (!self->path) {
        free(self);
        return NULL;
    }

    self->re = NULL;
    self->iter = new_base_listing(json);

    if (!self->iter) {
        free(self);
        return NULL;
    }

    return self;
}

raw_listing* raw_listing_url(char *path, char *url) {
    struct response *re = get_response(url);

    if (!re) {
        return NULL;
    }

    ju_json_t *json = ju_parse(re->content);

    if (!json) {
        free_response(re);
        return NULL;
    }

    raw_listing *self = raw_listing_data(path, json);

    if (!self) {
        ju_free(json);
        free_response(re);
        return NULL;
    }

    self->re = re;
    return self;
}

void raw_free_listing(raw_listing *self) {
    free_base_listing(self->iter);

    if (self->re) {
        free_response(self->re);
    }

    free(self->path);
    free(self);
}

struct raw_post* raw_listing_next(raw_listing *self) {
    int iter_next = base_listing_next(self->iter);

    if (iter_next < 0) {
        return NULL;
    }

    struct raw_post *post = raw_new_post(self->path, self->iter->json, iter_next);

    if (path_eq(self->path, post->path)) {
        raw_free_post(post);
        return raw_listing_next(self);
    } else {
        return post;
    }
}

struct raw_post* raw_listing_next_download(raw_listing *self) {
    struct raw_post *post = raw_listing_next(self);

    if (!post) {
        return NULL;
    }

    if (path_exists(post->path)) {
        return post;
    }

    int download_ret = raw_post_download(post);

    if (!download_ret) {
        return post;
    } else if (download_ret < 0) {
        raw_free_post(post);
        return raw_listing_next_download(self);
    } else {
        raw_free_post(post);
        return NULL;
    }
}

struct raw_post* raw_listing_next_no_repeat(raw_listing *self) {
    struct raw_post *post = raw_listing_next(self);

    if (!post) {
        return NULL;
    }

    if (path_exists(post->path)) {
        raw_free_post(post);
        return raw_listing_next_no_repeat(self);
    } else {
        return post;
    }
}

struct raw_post* raw_listing_next_no_repeat_download(raw_listing *self) {
    struct raw_post *post = raw_listing_next_no_repeat(self);

    if (!post) {
        return NULL;
    }

    int download_ret = raw_post_download(post);

    if (!download_ret) {
        return post;
    } else if (download_ret < 0) {
        raw_free_post(post);
        return raw_listing_next_no_repeat_download(self);
    } else {
        raw_free_post(post);
        return NULL;
    }
}

struct raw_post* raw_listing_flags_next_download(raw_listing *self, int flags) {
    if (flags & RAW_NO_REPEAT) {
        return raw_listing_next_no_repeat_download(self);
    } else {
        return raw_listing_next_download(self);
    }
}

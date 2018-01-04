#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "raw.h"
#include "path.h"
#include "get.h"
#include "reg.h"

struct raw_post* raw_new_post(char *parent_path, ju_json_t *json, int index) {
    struct raw_post *self = malloc(sizeof(struct raw_post));

    if (!self) {
        LOG_ERRNO();
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
    self->path = path_url_fname(parent_path, self->url);

    if (!self->path) {
        free(self->url);
        free(self);
        return NULL;
    }

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
        return NULL;
    }

    jsmntok_t tok = self->json->tokens[index];
    return regex_str_slice(self->json->json_str, tok.start, tok.end);
}

int raw_post_download(struct raw_post *self) {
    struct response *re = get_image(self->url);

    if (!re) {
        return -1;
    }

    int result = get_download_response(re, self->path);
    free_response(re);

    if (!result) {
        DEBUG("Collected new image: %s", self->url);
    }

    return result;
}

static void log_attribute(struct raw_post *self, char *key) {
    char *value = raw_post_data_get(self, key);
    if (!value) return;
    LOG("%s: %s", key, value);
    free(value);
}

void raw_post_log(struct raw_post *self) {
    log_attribute(self, "title");
    log_attribute(self, "permalink");
    log_attribute(self, "url");
}

static struct raw_base_listing* new_base_listing(ju_json_t *json) {
    struct raw_base_listing *self = malloc(sizeof(struct raw_base_listing));

    if (!self) {
        LOG_ERRNO();
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
    rp_deep_free(&self->popper, free);
    free(self);
}

static int base_listing_next(struct raw_base_listing *self, int random) {
    int *iter_next;

    if (random) {
        iter_next = rp_pop_random(&self->popper);
    } else {
        iter_next = rp_pop_index(&self->popper, 0);
    }

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
        LOG_ERRNO();
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

    self->free_json = 0;
    self->existing_posts = NULL;
    self->new_posts = NULL;
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
    self->free_json = 1;
    return self;
}

void raw_free_listing(raw_listing *self) {
    rp_deep_free(&self->new_posts, (rp_free_func) raw_free_post);
    rp_shallow_free(&self->existing_posts);

    if (self->free_json) {
        ju_free(self->iter->json);
    }

    free_base_listing(self->iter);

    if (self->re) {
        free_response(self->re);
    }

    free(self->path);
    free(self);
}

static struct raw_post* listing_next_post(raw_listing *self, int random) {
    int iter_next = base_listing_next(self->iter, random);

    if (iter_next < 0) {
        return NULL;
    }

    struct raw_post *post = raw_new_post(self->path, self->iter->json, iter_next);

    if (!post) {
        return NULL;
    } else if (path_eq(self->path, post->path)) {
        return listing_next_post(self, random);
    } else if (!rp_append(&self->new_posts, post)) {
        raw_free_post(post);
        return NULL;
    }

    if (path_exists(post->path)) {
        DEBUG("Already downloaded: %s", post->url);

        if (!rp_append(&self->existing_posts, post)) {
            return NULL;
        }
    }

    return post;
}

static struct raw_post* listing_next_download(raw_listing *self, int random) {
    struct raw_post *post = listing_next_post(self, random);

    if (!post) {
        return NULL;
    } else if (path_exists(post->path)) {
        return post;
    }

    int download_ret = raw_post_download(post);

    if (!download_ret) {
        return post;
    } else if (download_ret < 0) {
        return listing_next_download(self, random);
    } else {
        return NULL;
    }
}

static struct raw_post* listing_next_no_repeat(raw_listing *self, int random) {
    struct raw_post *post = listing_next_post(self, random);

    if (!post) {
        return NULL;
    } else if (path_exists(post->path)) {
        return listing_next_no_repeat(self, random);
    } else {
        return post;
    }
}

static struct raw_post* listing_next_no_repeat_download(raw_listing *self, int random) {
    struct raw_post *post = listing_next_no_repeat(self, random);

    if (!post) {
        return NULL;
    }

    int download_ret = raw_post_download(post);

    if (!download_ret) {
        return post;
    } else if (download_ret < 0) {
        return listing_next_no_repeat_download(self, random);
    } else {
        return NULL;
    }
}

typedef struct raw_post* (*listing_next_func_t)(raw_listing *self, int random);

static listing_next_func_t get_next_func(int flags) {
    if (flags & RAW_NO_REPEAT) {
        if (flags & RAW_DOWNLOAD) {
            return listing_next_no_repeat_download;
        }

        return listing_next_no_repeat;
    } else if (flags & RAW_DOWNLOAD) {
        return listing_next_download;
    }

    return listing_next_post;
}

struct raw_post* raw_listing_next(raw_listing *self, int flags) {
    return get_next_func(flags)(self, flags & RAW_RANDOM);
}

struct listing_fallback {
    char *path;
    struct raw_post *post;
};

#define __LISTING_FALLBACK(path_, post_) \
    (struct listing_fallback) { \
        .path = path_, \
        .post = post_, \
    }

static struct raw_post* get_existing_post_from_path(raw_listing *self, char *path) {
    rp_t *item;
    struct raw_post *post;

    for (item = self->existing_posts; item; item = item->next) {
        post = item->data;

        if (path_eq(path, post->path)) {
            return post;
        }
    }

    return NULL;
}

static struct listing_fallback fallback_random(raw_listing *self) {
    char *path = path_random_file(self->path);
    struct raw_post *post = NULL;

    if (path) {
        post = get_existing_post_from_path(self, path);
    }

    return __LISTING_FALLBACK(path, post);
}

static struct listing_fallback fallback_flags(raw_listing *self, int flags) {
    if (flags & RAW_NEW) {
        DEBUG("Falling back on image from new");
        struct raw_post *post = rp_pop_random(&self->existing_posts);

        if (!post) {
            goto fail;
        }

        return __LISTING_FALLBACK(strdup(post->path), post);
    }

    if (flags & RAW_ALL) {
        DEBUG("Falling back on image from all");
        return fallback_random(self);
    }

fail:
    return __LISTING_FALLBACK(NULL, NULL);
}

char* raw_listing_next_fallback(raw_listing *self, int flags) {
    struct raw_post *post = raw_listing_next(self, flags);
    char *path = NULL;

    if (!post) {
        DEBUG("Collection failed: %s", self->re->url);
        struct listing_fallback fallback = fallback_flags(self, flags);
        path = fallback.path;
        post = fallback.post;
    }

    if (post) {
        raw_post_log(post);

        if (path) {
            free(path);
        }

        path = strdup(post->path);
    }

    return path;
}

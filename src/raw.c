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
    struct response *im = get_image(self->url);

    if (!im) {
        return -1;
    }

    int file = path_open_write(self->path);

    if (file < 0) {
        free_response(im);
        return 1;
    }

    if (write(file, im->content, im->length) < 0) {
        close(file);
        free_response(im);
        return 2;
    }

    close(file);
    free_response(im);
    return 0;
}

static struct raw_base_listing* new_base_listing(char *url) {
    struct raw_base_listing *self = malloc(sizeof(struct raw_base_listing));

    if (!self) {
        return NULL;
    }

    struct response *re = get_response(url);

    if (!re) {
        free(self);
        return NULL;
    }

    self->re = re;
    ju_json_t *json = ju_parse(self->re->content);

    if (!json) {
        free_response(self->re);
        free(self);
        return NULL;
    }

    self->json = json;
    int data_obj_i = ju_object_get(self->json, 0, "data");
    int posts_arr_i = ju_object_get(self->json, data_obj_i, "children");

    if (posts_arr_i < 1) {
        ju_free(json);
        free_response(self->re);
        free(self);
        return NULL;
    }

    self->popper = ju_array_rp(self->json, posts_arr_i);

    if (!self->popper) {
        ju_free(json);
        free_response(self->re);
        free(self);
        return NULL;
    }

    return self;
}

static void free_base_listing(struct raw_base_listing *self) {
    rp_deep_free(&self->popper);
    ju_free(self->json);
    free_response(self->re);
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

raw_listing* raw_new_listing(char *path, char *url) {
    raw_listing *self = malloc(sizeof(raw_listing));

    if (!self) {
        return NULL;
    }

    self->path = path;

    struct raw_base_listing *iter = new_base_listing(url);

    if (!iter) {
        free(self);
        return NULL;
    }

    self->iter = iter;
    return self;
}

void raw_free_listing(raw_listing *self) {
    free_base_listing(self->iter);
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
    if (flags & NO_REPEAT) {
        return raw_listing_next_no_repeat_download(self);
    } else {
        return raw_listing_next_download(self);
    }
}

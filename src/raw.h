#ifndef RAW_H
#define RAW_H

#include "jsmnutils.h"
#include "get.h"

enum raw_flags {
    FAIL = 0,
    NO_REPEAT = 1 << 0
};

struct raw_post {
    ju_json_t *json;
    int index;
    char *path;
    char *url;
};

struct raw_post* raw_new_post(char *parent_path, ju_json_t *json, int index);
void raw_free_post(struct raw_post *self);
char* raw_post_data_get(struct raw_post *self, char *key);
int raw_post_download(struct raw_post *self);

struct raw_base_listing {
    ju_json_t *json;
    struct ju_random_iter *iter;
    struct response *re;
};

typedef struct {
    char *path;
    struct raw_base_listing *iter;
} raw_listing;

raw_listing* raw_new_listing(char *path, char *url);
void raw_free_listing(raw_listing *self);
struct raw_post* raw_listing_next(raw_listing *self);
struct raw_post* raw_listing_next_download(raw_listing *self);
struct raw_post* raw_listing_next_no_repeat(raw_listing *self);
struct raw_post* raw_listing_next_no_repeat_download(raw_listing *self);
struct raw_post* raw_listing_flags_next_download(raw_listing *self, int flags);
char* raw_listing_flags_next_recover(raw_listing *self, int flags);

#endif

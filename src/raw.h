#ifndef RAW_H
#define RAW_H

#include "jsmnutils.h"
#include "get.h"
#include "random_popper.h"

enum raw_flags {
    RAW_NO_REPEAT = 1 << 0,
    RAW_RANDOM = 1 << 1,
    RAW_DOWNLOAD = 1 << 2,
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
void raw_post_log(struct raw_post *self);

struct raw_base_listing {
    ju_json_t *json;
    rp_t *popper;
};

typedef struct {
    char *path;
    struct response *re;
    struct raw_base_listing *iter;
    int free_json;
    rp_t *existing_paths;
} raw_listing;

raw_listing* raw_listing_data(char *path, ju_json_t *json);
raw_listing* raw_listing_url(char *path, char *url);
void raw_free_listing(raw_listing *self);
struct raw_post* raw_listing_next(raw_listing *self, int flags);

#endif

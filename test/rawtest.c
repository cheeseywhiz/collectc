#include <string.h>
#include <stdio.h>

#include "raw.h"
#include "jsmnutils.h"
#include "path.h"
#include "test.h"

SMALL_TEST test_new_post(struct raw_post *post) {
    SCORE_INIT();
    ASSERT(post->index == 7);
    ASSERT(path_eq(post->path, "cache/test/googlelogo_color_272x92dp.png"));
    char *post_title = raw_post_data_get(post, "title");
    ASSERT(!strcmp(post_title, "google"));
    free(post_title);
    RETURN_SCORE();
}

SMALL_TEST test_listing_next(raw_listing *listing) {
    SCORE_INIT();
    struct raw_post *next = raw_listing_next(listing, 0);
    SUBSCORE(test_new_post(next));
    char *next_path = "cache/test/archlinux-logo-dark-90dpi.ebdee92a15b3.png";
    path_touch(next_path);
    raw_free_post(next);
    next = raw_listing_next(listing, RAW_NO_REPEAT);
    char *next_title = raw_post_data_get(next, "title");
    ASSERT(!strcmp(next_title, "reddit"));
    free(next_title);
    raw_free_post(next);
    RETURN_SCORE();
}

BIG_TEST raw_test_main(void) {
    SCORE_INIT();
    char *dir = "cache/test";
    path_mkdir(dir, MK_MODE_755, MK_PARENTS);
    char *data = "{\"data\": {\"children\": [{\"data\": {\"title\": \"google\", \"url\": \"https://www.google.com/images/branding/googlelogo/1x/googlelogo_color_272x92dp.png\"}}, {\"data\": {\"title\": \"archlinux\", \"url\": \"https://www.archlinux.org/static/logos/archlinux-logo-dark-90dpi.ebdee92a15b3.png\"}}, {\"data\": {\"title\": \"reddit\", \"url\": \"https://i.reddituploads.com/d1e77b5c62694624ba7235a57431f070?fit=max&h=1536&w=1536&s=b3103272b2bf369f5c42396b09c4caf8\"}}]}}";
    ju_json_t *json = ju_parse(data);
    struct raw_post *post = raw_new_post(dir, json, 7);
    SMALL_REPORT("raw_new_post()", test_new_post(post));
    raw_listing *listing = raw_listing_data(dir, json);
    SMALL_REPORT("raw_listing_next()", test_listing_next(listing));
    raw_free_listing(listing);
    raw_free_post(post);
    ju_free(json);
    RETURN_SCORE();
}

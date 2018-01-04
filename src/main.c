#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "collect.h"

int main(int argc, char **argv) {
    SET_LOG_LEVEL(LOG_INFO);
    rand_reseed();
    char *reddit_url;

    if (argc < 2) {
        reddit_url = "https://www.reddit.com/r/EarthPorn/hot/.json?limit=10";
    } else {
        reddit_url = argv[1];
    }

    char *dir = path_norm("cache");

    if (path_mkdir(dir, MK_MODE_755, MK_PARENTS | MK_EXISTS_OK)) {
        free(dir);
        return 1;
    }

    char *path;

    if (!strcmp(reddit_url, "random")) {
        path = path_random_file(dir);
        free(dir);

        if (!path) {
            return 1;
        }

        LOG("file: %s", path);
        printf("%s\n", path);
        free(path);
        return 0;
    }

    raw_listing *posts = raw_listing_url(dir, reddit_url);

    if (!posts) {
        free(dir);
        return 1;
    }

    int flags = RAW_NO_REPEAT | RAW_RANDOM | RAW_DOWNLOAD | RAW_NEW | RAW_ALL;
    path = raw_listing_next_fallback(posts, flags);

    if (!path) {
        raw_free_listing(posts);
        free(dir);
        return 1;
    }

    printf("%s\n", path);
    free(path);
    raw_free_listing(posts);
    free(dir);
    return 0;
}

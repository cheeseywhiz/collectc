#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "raw.h"
#include "path.h"
#include "rand.h"

int main(int argc, char **argv) {
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

    if (!strcmp(reddit_url, "random")) {
        char *path = path_random_file(dir);
        free(dir);

        if (!path) {
            return 1;
        }

        printf("%s\n", path);
        free(path);
        return 0;
    }

    raw_listing *posts = raw_listing_url(dir, reddit_url);

    if (!posts) {
        free(dir);
        return 1;
    }

    int flags = RAW_NO_REPEAT | RAW_RANDOM | RAW_DOWNLOAD;
    struct raw_post *post = raw_listing_next(posts, flags);

    if (!post) {
        raw_free_listing(posts);
        free(dir);
        return 1;
    }

    printf("%s\n", post->path);
    raw_free_post(post);
    raw_free_listing(posts);
    free(dir);
    return 0;
}

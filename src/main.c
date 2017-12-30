#include <stdio.h>
#include <string.h>

#include "raw.h"
#include "path.h"

int main(int argc, char **argv) {
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
        char *path = path_random(dir);
        free(dir);

        if (!path) {
            return 1;
        }

        printf("%s\n", path);
        free(path);
        return 0;
    }

    raw_listing *posts = raw_new_listing(dir, reddit_url);

    if (!posts) {
        free(dir);
        return 1;
    }

    struct raw_post *post = raw_listing_flags_next_download(posts, NO_REPEAT);

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

#include <stdio.h>

#include "raw.h"
#include "path.h"

int main(int argc, char **argv) {
    char *reddit_url;

    if (argc < 2) {
        reddit_url = "https://www.reddit.com/r/EarthPorn/hot/.json?limit=10";
    } else {
        reddit_url = argv[1];
    }

    char *dir = path_norm(".");

    if (path_mkdir(dir, MODE_DEF, EXISTS_OK_DEF)) {
        return 1;
    }

    raw_listing *posts = raw_new_listing(dir, reddit_url);

    if (!posts) {
        return 1;
    }

    struct raw_post *post = raw_listing_flags_next_download(posts, NO_REPEAT);

    if (!post) {
        raw_free_listing(posts);
        return 1;
    }

    printf("%s\n", post->path);
    raw_free_post(post);
    raw_free_listing(posts);
    free(dir);
    return 0;
}

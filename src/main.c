#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "collect.h"

char *usage = "collect {reddit,random} [-ahnrvV] [-o=DIR] [-u=URL]\n\
\n\
Collect an image from a Reddit URL.\n\
\n\
subcommands:\n\
\treddit\tdownload a new image\n\
\trandom\tprint a random path from -o\n\
\n\
flags:\n\
\t-a\tfallback all\n\
\t-h\thelp\n\
\t-n\tfallback new\n\
\t-r\tno repeat\n\
\t-v\tverbosity\n\
\t-V\tversion\n\
\n\
parameters:\n\
\t-o=~/.cache/collectc\n\
\t\toutput directory where the images go\n\
\t-u=https://www.reddit.com/r/EarthPorn/hot/.json?limit=10\n\
\t\treddit post listing URL";

typedef char* (*subcommand_func)(char *dir, char *url, int raw_flags);

static char* reddit_subcommand(char *dir, char *url, int raw_flags) {
    raw_listing *listing = raw_listing_url(dir, url);

    if (!listing) {
        return NULL;
    }

    char *path = raw_listing_next_fallback(listing, raw_flags);
    raw_free_listing(listing);
    return path;
}

static char* random_subcommand(char *dir, __attribute__((unused)) char *url, __attribute__((unused)) int raw_flags) {
    return path_random_file(dir);
}

static subcommand_func get_subcommand_func(char *subcommand) {
    if (!strcmp(subcommand, "reddit")) {
        return reddit_subcommand;
    } else if (!strcmp(subcommand, "random")) {
        return random_subcommand;
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    log_init_prog_name(argv);
    rand_reseed();
    char *dir = "~/.cache/collectc";
    char *url = "https://www.reddit.com/r/EarthPorn/hot/.json?limit=10";
    int raw_flags = RAW_RANDOM | RAW_DOWNLOAD;
    int verbosity = 0;
    int flag;
    opterr = 0;

    if (argc == 1) {
        printf("%s\n", usage);
        return 0;
    }

    subcommand_func subcommand = get_subcommand_func(argv[1]);

    switch(getopt(argc, argv, "hV")) {
        case 'h':
            printf("%s\n", usage);
            return 0;
        case 'V':
            printf("%s\n", COLLECT_VERSION);
            return 0;
    }

    while ((flag = getopt(argc, argv, "ahnrvVo:u:")) != -1) {
        switch (flag) {
            case 'a':
                raw_flags |= RAW_ALL;
                break;
            case 'h':
                printf("%s\n", usage);
                return 0;
            case 'n':
                raw_flags |= RAW_NEW;
                break;
            case 'r':
                raw_flags |= RAW_NO_REPEAT;
                break;
            case 'v':
                verbosity++;
                break;
            case 'V':
                printf("%s\n", COLLECT_VERSION);
                return 0;
            case 'o':
                dir = optarg;
                break;
            case 'u':
                url = optarg;
                break;
            case '?':
                ERROR("Unknown option: -%c", optopt);
                return 1;
        }
    }

    if (verbosity == 0) {
        SET_LOG_LEVEL(LOG_WARNING);
    } else if (verbosity == 1) {
        SET_LOG_LEVEL(LOG_INFO);
    } else if (verbosity >= 2) {
        SET_LOG_LEVEL(LOG_DEBUG);
    }


    if (!subcommand) {
        printf("%s\n", usage);
        return 1;
    }

    if (path_mkdir(dir, MK_MODE_755, MK_EXISTS_OK)) {
        return 1;
    }

    char *path = subcommand(dir, url, raw_flags);

    if (!path) {
        return 1;
    }

    INFO("file: %s", path);
    printf("%s\n", path);
    free(path);
    return 0;
}

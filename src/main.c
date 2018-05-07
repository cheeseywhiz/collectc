#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "collect.h"

char *usage = "collect [-hV] {reddit,random,clear} [-adnrv] [-o DIR] [-u URL]\n\
\n\
Collect an image from a Reddit URL.\n\
\n\
subcommands:\n\
\treddit\tdownload a new image\n\
\trandom\tprint a random path from -o\n\
\tclear\trecursive remove -o\n\
\n\
options:\n\
\t-a\tfallback all\n\
\t-d\tdry run; don't download\n\
\t-h\thelp\n\
\t-n\tfallback new\n\
\t-r\tno repeat\n\
\t-v\tverbosity\n\
\t-V\tversion\n\
\n\
parameters:\n\
\t-o ~/.cache/collectc\n\
\t\toutput directory where the images go\n\
\t-u https://www.reddit.com/r/EarthPorn/hot/.json?limit=10\n\
\t\treddit post listing URL";

typedef char* (*subcommand_func)(char *dir, char *url, int raw_flags);

static char* reddit_subcommand(char *dir, char *url, int raw_flags) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    raw_listing listing;

    if (raw_init_listing_url(&listing, dir, url)) {
        return NULL;
    }

    char *path = raw_listing_next_fallback(&listing, raw_flags);
    raw_free_listing(&listing);
    curl_global_cleanup();
    return path;
}

static char* random_subcommand(char *dir, __attribute__((unused)) char *url, __attribute__((unused)) int raw_flags) {
    return path_random_file(dir);
}

static char* clear_subcommand(char *dir, __attribute__((unused)) char *url, __attribute__((unused)) int raw_flags) {
    if (path_rm_tree(dir)) {
        return NULL;
    }

    return strdup(dir);
}

static subcommand_func get_subcommand_func(char *subcommand) {
    if (!strcmp(subcommand, "reddit")) {
        return reddit_subcommand;
    } else if (!strcmp(subcommand, "random")) {
        return random_subcommand;
    } else if (!strcmp(subcommand, "clear")) {
        return clear_subcommand;
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    log_init_prog_name(argv);
    rand_reseed();
    char *dir = "~/.cache/collectc";
    char *url = "r/EarthPorn/hot/.json?limit=10";
    raw_flags flags = RAW_RANDOM;
    int verbosity = 0;
    int dry_run = 0;
    int opt;
    opterr = 0;

    if (argc == 1) {
        printf("%s\n", usage);
        return 0;
    }

    subcommand_func subcommand = get_subcommand_func(argv[1]);

    switch(getopt(2, argv, "hV")) {
        case 'h':
            printf("%s\n", usage);
            return 0;
        case 'V':
            printf("%s\n", COLLECT_VERSION);
            return 0;
    }

    while ((opt = getopt(argc, argv, "adnrvo:u:")) != -1) {
        switch (opt) {
            case 'a':
                flags |= RAW_ALL;
                break;
            case 'd':
                dry_run = 1;
                break;
            case 'n':
                flags |= RAW_NEW;
                break;
            case 'r':
                flags |= RAW_NO_REPEAT;
                break;
            case 'v':
                verbosity++;
                break;
            case 'o':
                dir = optarg;
                break;
            case 'u':
                url = optarg;
                break;
            case '?':
                ERROR("Unknown option: -%c", optopt);
                return 1;
            default:
                printf("%s\n", usage);
                return 1;
        }
    }

    if (!dry_run) {
        flags |= RAW_DOWNLOAD;
    }

    if (verbosity == 0) {
        SET_LOG_LEVEL(LOG_WARNING);
    } else if (verbosity == 1) {
        SET_LOG_LEVEL(LOG_INFO);
    } else if (verbosity >= 2) {
        SET_LOG_LEVEL(LOG_DEBUG);
    }

    dir = path_norm(dir);

    if (!dir) {
        return 1;
    }

    int exit_val = 0;

    if (!subcommand) {
        printf("%s\n", usage);
        exit_val = 1;
        goto exit;
    }

    if (!dry_run && path_mkdir(dir, MK_MODE_755, MK_EXISTS_OK)) {
        exit_val = 1;
        goto exit;
    }

    char *path = subcommand(dir, url, flags);

    if (!path) {
        exit_val = 1;
        goto exit;
    }

    if (subcommand != clear_subcommand) {
        INFO("file: %s", path);
        printf("%s\n", path);
    }

    free(path);

exit:
    free(dir);
    return exit_val;
}

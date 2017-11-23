#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "get.h"
#include "reg.h"
#include "jsmnutils.h"

int main(int argc, char **argv) {
    char *reddit_url;

    if (argc < 2) {
        reddit_url = "https://www.reddit.com/r/EarthPorn/hot/.json?limit=10";
    } else {
        reddit_url = argv[1];
    };

    struct response *re = get_response(reddit_url);

    if (!re) {
        fprintf(stderr, "get_response failed\n");
        return 1;
    };

    ju_json_t *json = ju_parse(re->content);

    if (!json) {
        fprintf(stderr, "ju_parse failed\n");
        free_response(re);
        return 1;
    };

    struct ju_array_iter *urls = ju_init_url_iter(json);
    
    if (!urls) {
        fprintf(stderr, "ju_init_url_iter failed\n");
        ju_free(json);
        free_response(re);
        return 1;
    };
    
    char *url;
    int break_ = 0;

    int flags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    for (url = ju_next_url(urls); url; url = ju_next_url(urls)) {
        struct response *im = get_response(url);

        if (regex_starts_with(im->type, "image")) {
            char *path = regex_url_fname(url);
            printf("%s\n", path);

            int im_fd = open(path, flags, mode);
            write(im_fd, im->content, im->length);
            close(im_fd);

            free(path);
            break_ = 1;
        };

        free_response(im);
        free(url);
        if (break_) break;
    };

    free(urls);
    ju_free(json);
    free_response(re);
    return 0;
}

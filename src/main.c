#include <stdlib.h>
#include <stdio.h>

#include "get.h"
#include "reg.h"
#include "jsmnutils.h"

int main(int argc, char **argv) {
    struct response *re = get_response("https://www.reddit.com/r/EarthPorn/hot/.json?limit=10");

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
        free_response(re);
        return 1;
    };
    
    char *url;

    for (url = ju_next_url(urls); url; url = ju_next_url(urls)) {
        struct response *im = get_response(url);
        if (regex_starts_with(im->type, "image")) {
            fprintf(stderr, "%s\n", url);
            free_response(im);
            free(url);
            break;
        } else {
            free_response(im);
            free(url);
        };
    };

    free(urls);
    ju_free(json);
    free_response(re);
    return 0;
};

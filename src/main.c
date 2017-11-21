#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "get.h"
#include "reg.h"
#include "jsmnutils.h"

// int main(int argc, char **argv) {
int main(void) {
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
        ju_free(json);
        free_response(re);
        return 1;
    };
    
    char *url;

    for (url = ju_next_url(urls); url; url = ju_next_url(urls)) {
        struct response *im = get_response(url);
        if (regex_starts_with(im->type, "image")) {
            fprintf(stderr, "%s\n", url);

            FILE *im_file = fopen("a.jpg", "w");
            int *ptr = (int *)im->content;
            fwrite(ptr, sizeof(ptr[0]), sizeof(ptr)/sizeof(ptr[0]), im_file);
            fclose(im_file);

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
}

/*
 *  Test various ways of iterating through a parsed json set
 */

#include <stdlib.h>
#include <stdio.h>

#include "get.h"
#include "reg.h"
#include "jsmnutils.h"

int iter(void) {
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

    for (int i = 0; i < json->n_tokens; i++) {
        jsmntok_t token = json->tokens[i];
        char *tok_str = regex_str_slice(json->json_str, token.start, token.end);

        if (tok_str) {
            printf("%s\n", tok_str);
            free(tok_str);
        } else {
            printf("tok_str evaluated false\n");
        };
    };

    int data = ju_object_get(json, 0, "data");
    int posts = ju_object_get(json, data, "children");
    printf("data = %d;\n", data);
    printf("posts = %d;\n", posts);

    struct ju_array_iter *iter = ju_init_array_iter(json, posts);
    
    if (!iter) {
        fprintf(stderr, "ju_init_array_iter failed\n");
        ju_free(json);
        free_response(re);
        return 1;
    };
    
    int i;

    for (i = ju_array_next(iter); i > 0; i = ju_array_next(iter)) {
        printf("t#%d\n", i);
        printf("p#%d\n\n", json->tokens[i].parent);
    };

    free(iter);

    struct ju_array_iter *urls = ju_init_url_iter(json);
    char *url;

    for (url = ju_next_url(urls); url; url = ju_next_url(urls)) {
        printf("%s\n", url);
        free(url);
    };

    free(urls);
    ju_free(json);
    free_response(re);
    return 0;
};

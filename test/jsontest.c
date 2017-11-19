#include <string.h>

#define JSMN_PARENT_LINKS
#include "jsmn.h"
#include "jsmnutils.h"
#include "reg.h"
#include "get.h"

int print_urls(char *json_str) {
    ju_json_t json;
    ju_parse(&json, json_str);

    int data_arr_i = ju_key_search(&json, 0, "data");
    int posts_arr_i = ju_key_search(&json, data_arr_i, "children");

    printf("i=%d\n", posts_arr_i);
    printf("size=%d\n", json.tokens[posts_arr_i].size);
    if (json.tokens[posts_arr_i].type == JSMN_ARRAY) {
        printf("is array\n");
    } else {
        printf("is not array\n");
    };

    struct ju_array_iter iter = ju_init_array_iter(&json, posts_arr_i);

    int i;
    int sub_data_i;
    int url_i;

    for (i = ju_array_next(&iter); i > 0; i = ju_array_next(&iter)) {
        sub_data_i = ju_key_search(&json, i, "data");
        url_i = ju_key_search(&json, sub_data_i, "url");

        for (int j = json.tokens[url_i].start; j < json.tokens[url_i].end; j++) {
            printf("%c", json.json_str[j]);
        };

        printf("\n");
    };

    return 0;
};

int main(void) {
    struct response *re = get_response("https://www.reddit.com/r/EarthPorn/hot/.json?limit=10");

    int exit = print_urls(re->content);

    free_response(re);
    return exit;
};

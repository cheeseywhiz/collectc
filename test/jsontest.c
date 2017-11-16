#include "jsmn.h"
// #include "reg.h"
#include "get.h"

int main(void) {
    struct response *re = get_response("https://www.reddit.com/r/all/hot/.json?limit=10");

    if (re == NULL) {
        return 1;
    };

    jsmn_parser parser;
    int max_tokens = 32768;
    jsmn_init(&parser);

    int n_tokens = max_tokens; /* jsmn_parse(&parser, re->content, re->length, NULL, max_tokens); */
    jsmntok_t tokens[n_tokens];
    int actual = jsmn_parse(&parser, re->content, re->length, tokens, max_tokens);

    printf("len=%d\n", actual); // -2

    for (int i = 0; i < actual; i++) {
        printf("%d\n", tokens[i].type);
    };

    free_response(re);
    return 0;
};

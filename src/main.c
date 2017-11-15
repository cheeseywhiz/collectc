#include <stdlib.h>

#include "string.h"
#include "get.h"

#define REDDIT_URL "https://www.reddit.com/r/EarthPorn/hot/.json?limit=10"

int main(int argc, char **argv) {
    struct response *re = new_response();
    int ret_code = get(re, REDDIT_URL);
    printf("%s\n", re->type);
    free_response(re);
    return ret_code;
};

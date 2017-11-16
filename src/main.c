#include <stdlib.h>

#include "string.h"
#include "get.h"

#define REDDIT_URL "https://www.reddit.com/r/EarthPorn/hot/.json?limit=10"

int main(int argc, char **argv) {
    struct response *re = get_response(REDDIT_URL);

    if (re == NULL) {
        return 1;
    };

    printf("%s\n", re->type);
    free_response(re);
    return 0;
};

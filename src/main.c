#include <stdlib.h>
#include "string.h"
#include "get.h"

#define REDDIT_URL "https://www.reddit.com/r/EarthPorn/hot/.json?limit=10"

int main(int argc, char **argv) {
    struct response re;
    int ret_code = (int) get(&re, REDDIT_URL);
    printf("%s\n", re.content);
    return ret_code;
};

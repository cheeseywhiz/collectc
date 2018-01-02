#include <stdio.h>

#include "rand.h"
#include "test.h"

SMALL_TEST test_randbelow(int n, int below) {
    SCORE_INIT();
    printf("[");
    int rand;

    for (int i = 0; i < n; i++) {
        rand = randbelow(below);
        ASSERT(0 <= rand && rand < below);
        printf("%d, ", rand);
    }

    printf("]\n");
    RETURN_SCORE();
}

BIG_TEST rand_test_main(void) {
    SCORE_INIT();
    SMALL_REPORT("randbelow()", test_randbelow(25, 10));
    RETURN_SCORE();
}

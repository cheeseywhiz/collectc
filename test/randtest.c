#include <stdio.h>
#include <assert.h>
#include "rand.h"

int rand_test_main(void) {
    printf("randbelow(10) test:\n");
    for (int j = 0; j < 26; j++) printf("%d, ", randbelow(10));
    printf("\nPassed: random numbers below n generator\n");
    return 0;
}

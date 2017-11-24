#include <stdio.h>
#include <assert.h>
#include "rand.h"

int randtest(void) {
    printf("randbelow(10) test:\n");
    for (int j = 0; j < 26; j++) printf("%d, ", randbelow(10));
    printf("\nPassed: random numbers below n generator\n");

    int_list *list = int_list_random_order(20);
    assert(list->length == 20);

    if (!list) {
        fprintf(stderr, "randtest list int_list_random_order failed\n");
        return 1;
    }

    printf("First 20 non negative integers in random order:\n");

    for (int i = 0; i < list->length; i++) {
        printf("%d\t%d\n", i, list->items[i]);
    }

    free_int_list(list);
    printf("Passed: randomized sequence\n");
    return 0;
}

#include <stdio.h>
#include "rand.h"

int randtest(void) {
    int_list *list = int_list_random_order(100);

    if (!list) {
        fprintf(stderr, "randtest list int_list_random_order failed\n");
        return 1;
    };

    for (int i = 0; i < list->length; i++) {
        printf("%d:%d\n", i, list->items[i]);
    };

    free_int_list(list);
    for (int j = 0; j < 10; j++) printf("%u\n", urandom_number());
    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#include "random_popper.h"

int rp_test_main(void) {
    printf("rp_init(10):\n");
    rp_t *element, *popper = rp_init(10);

    for (element = popper; element; element = element->next) {
        printf("%d\n", element->num);
    }

    printf("rp_pop_random():\n");
    int i, j = 0;

    for (i = rp_pop_random(&popper); i >= 0; i = rp_pop_random(&popper)) {
        printf("%d\t%d\n", j++, i);
    }

    printf("second iteration should be empty:\n");

    for (element = popper; element; element = element->next) {
        printf("%d\n", element->num);
    }

    if (!popper) {
        printf("popper is NULL- check\n");
        return 0;
    } else {
        printf("popper should be NULL after popping all\n");
        rp_free(&popper);
        return 1;
    }
}

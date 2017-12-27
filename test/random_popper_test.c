#include <stdio.h>
#include <stdlib.h>

#include "random_popper.h"

int rp_test_main(void) {
    printf("rp_init(20):\n");
    rp_t *element, *popper = rp_init(20);

    for (element = popper; element; element = element->next) {
        printf("%d\n", element->num);
    }

    printf("rp_pop_random():\n");
    int i, j = 0;

    for (i = rp_pop_random(&popper); i >= 0; i = rp_pop_random(&popper)) {
        printf("%d\t%d\n", j++, i);
    }

    return 0;
}

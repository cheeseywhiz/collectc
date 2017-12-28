#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "random_popper.h"
#include "test.h"

SSSCORE test_append(rp_t **popper) {
    SCORE_INIT();

    for (int i = 0; i < 10; i++) {
        ASSERT(rp_append(popper, new_int(i)));
    }

    RETURN_SCORE();
}

SSSCORE test_len(rp_t **popper) {
    SCORE_INIT();
    ASSERT(rp_len(popper) == 10);
    RETURN_SCORE();
}

SSSCORE test_get_index(rp_t **popper) {
    SCORE_INIT();
    int n_cases = 4;
    int cases[] = {1, 3, 5, 8};

    for (int i = 0; i < n_cases; i++) {
        int case_ = cases[i];
        int *actual = rp_get_index(popper, case_)->data;
        ASSERT(case_ == *actual);
    }

    RETURN_SCORE();
}

SSSCORE test_last(rp_t **popper) {
    SCORE_INIT();
    int *value = rp_last(popper)->data;
    ASSERT(*value == 9);
    RETURN_SCORE();
}

SSSCORE test_pop_index(rp_t **popper) {
    SCORE_INIT();
    int index = 4;
    size_t length = rp_len(popper);
    int *value_get = rp_get_index(popper, index)->data;
    int *value_pop = rp_pop_index(popper, index);
    ASSERT(*value_get == *value_pop);
    ASSERT(--length == rp_len(popper));
    free(value_pop);
    RETURN_SCORE();
}

SSSCORE test_pop_random(rp_t **popper) {
    SCORE_INIT();
    size_t length = rp_len(popper);
    int *elem_num;
    printf("[");

    for (elem_num = rp_pop_random(popper); elem_num; elem_num = rp_pop_random(popper)) {
        printf("%d, ", *elem_num);
        free(elem_num);
        ASSERT(--length == rp_len(popper));
    }

    printf("]\n");
    RETURN_SCORE();
}

SSSCORE null_check(rp_t **popper) {
    SCORE_INIT();
    int i = 0;
    rp_t *element;
    for (element = *popper; element; element = element->next) i++;
    ASSERT(i == 0);

    if (*popper) {
        rp_shallow_free(popper);
    }

    RETURN_SCORE();
}

struct score rp_test_main(void) {
    MODULE_INIT();
    rp_t *popper = NULL;
    FUNCTION_REPORT("rp_append(0..9)", test_append(&popper));
    FUNCTION_REPORT("rp_len()", test_len(&popper));
    FUNCTION_REPORT("rp_get_index()", test_get_index(&popper));
    FUNCTION_REPORT("rp_last()", test_last(&popper));
    FUNCTION_REPORT("rp_pop_index()", test_pop_index(&popper));
    FUNCTION_REPORT("rp_pop_random()", test_pop_random(&popper));
    FUNCTION_REPORT("popper NULL check", null_check(&popper));
    MODULE_EXIT();
}

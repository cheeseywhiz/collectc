#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "jsmnutils.h"
#include "reg.h"
#include "random_popper.h"
#include "test.h"

SMALL_TEST test_parse(ju_json_t *json) {
    SCORE_INIT();
    ASSERT(json->n_tokens == 13);
    RETURN_SCORE();
}

SMALL_TEST test_object_get(ju_json_t *json) {
    SCORE_INIT();
    int sub_object = ju_object_get(json, 0, "object");
    ASSERT(sub_object == 2);
    int value2tok = ju_object_get(json, sub_object, "key1");
    ASSERT(value2tok == 4);
    char *value2str = regex_str_slice(json->json_str, json->tokens[value2tok].start, json->tokens[value2tok].end);
    ASSERT(strcmp("value2", value2str) == 0);
    free(value2str);
    RETURN_SCORE();
}

SMALL_TEST test_array_init(struct ju_array_iter *iter) {
    SCORE_INIT();
    ASSERT(iter->size == 3);
    RETURN_SCORE();
}

SMALL_TEST test_array_next(struct ju_array_iter *iter) {
    SCORE_INIT();
    int expected_values[] = {1, 5, 11};
    int actual, i = 0;

    for (actual = ju_array_next(iter); actual >= 0; actual = ju_array_next(iter)) {
        ASSERT(actual == expected_values[i++]);
    }

    RETURN_SCORE();
}

SMALL_TEST test_array_rp(ju_json_t *json) {
    SCORE_INIT();
    int expected_values[] = {7, 8, 9, 10};
    rp_t *popper = ju_array_rp(json, 6);
    rp_t *index;
    int i = 0;
    int *actual;

    for (index = popper; index; index = index->next) {
        int expected = expected_values[i++];
        actual = index->data;
        ASSERT(expected == *actual);
    }

    rp_deep_free(&popper);
    RETURN_SCORE();
}

BIG_TEST ju_test_main(void) {
    SCORE_INIT();
    char *data = "{\"object\": {\"key1\": \"value2\"}, \"array\": [\"value3\", \"value4\", \"value5\", \"value6\"], \"key2\": \"value7\"}";
    ju_json_t *json = ju_parse(data);
    SMALL_REPORT("ju_parse()", test_parse(json));
    SMALL_REPORT("ju_object_get()", test_object_get(json));
    SMALL_REPORT("ju_array_rp()", test_array_rp(json));
    struct ju_array_iter *iter = ju_array_init(json, 0);
    SMALL_REPORT("ju_array_init()", test_array_init(iter));
    SMALL_REPORT("ju_array_next()", test_array_next(iter));
    free(iter);
    ju_free(json);
    RETURN_SCORE();
}

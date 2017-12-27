#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "jsmnutils.h"
#include "reg.h"
#include "random_popper.h"

ju_json_t* test_parse(void) {
    char *data = "{\"object\": {\"key1\": \"value2\"}, \"array\": [\"value3\", \"value4\", \"value5\", \"value6\"], \"key2\": \"value7\"}";
    ju_json_t *json = ju_parse(data);
    assert(json->n_tokens == 13);
    printf("Passed: parsed json data\n");
    return json;
}

void test_object_get(ju_json_t *json) {
    int sub_object = ju_object_get(json, 0, "object");
    assert(sub_object == 2);
    int value2tok = ju_object_get(json, sub_object, "key1");
    assert(value2tok == 4);
    char *value2str = regex_str_slice(json->json_str, json->tokens[value2tok].start, json->tokens[value2tok].end);
    assert(strcmp("value2", value2str) == 0);
    free(value2str);
    printf("Passed: manipulated json data\n");
}

struct ju_array_iter* test_array_init(ju_json_t *json) {
    struct ju_array_iter *iter = ju_array_init(json, 0);
    assert(iter->size == 3);
    printf("Passed: created json iterartor\n");
    return iter;
}

void test_array_next(struct ju_array_iter *iter) {
    int index;
    index = ju_array_next(iter);
    assert(index == 1);
    index = ju_array_next(iter);
    assert(index == 5);
    index = ju_array_next(iter);
    assert(index == 11);
    index = ju_array_next(iter);
    assert(index < 0);
    printf("Passed: iterated through json arrays and objects\n");
}

void test_array_rp(ju_json_t *json) {
    rp_t *popper = ju_array_rp(json, 6);
    assert(rp_get_index(&popper, 0)->num == 7);
    assert(rp_get_index(&popper, 1)->num == 8);
    assert(rp_get_index(&popper, 2)->num == 9);
    assert(rp_get_index(&popper, 3)->num == 10);
    printf("Passed: made sequence from JSON array\n");
    rp_free(&popper);
}

int ju_test_main(void) {
    ju_json_t *json = test_parse();
    test_object_get(json);
    struct ju_array_iter *iter = test_array_init(json);
    test_array_next(iter);
    free(iter);
    test_array_rp(json);
    ju_free(json);
    return 0;
}

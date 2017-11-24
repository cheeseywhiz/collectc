#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "jsmnutils.h"
#include "reg.h"

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

struct ju_random_iter* test_random_init(ju_json_t *json) {
    struct ju_random_iter *iter = ju_random_init(json, 6);
    assert(iter->list[0] == 7);
    assert(iter->list[1] == 8);
    assert(iter->list[2] == 9);
    assert(iter->list[3] == 10);
    printf("Passed: made sequence from array iterator\n");
    return iter;
}

void test_random_next(struct ju_random_iter *iter) {
    int i;
    printf("Random order: ");
    for (i = ju_random_next(iter); i > 0; i = ju_random_next(iter)) printf("%d ", i);
    printf("\nPassed: iterated randomly through array\n");
}

struct ju_array_iter* test_url_init(ju_json_t *json) {
    struct ju_array_iter *iter = ju_url_init(json);
    assert(iter->array_i == 4);
    printf("Passed: located posts structure array\n");
    return iter;
}

void test_url_next(struct ju_array_iter *iter) {
    char *url;
    url = ju_url_next(iter);
    assert(0 == strcmp(url, "https://www.google.com/images/branding/googlelogo/1x/googlelogo_color_272x92dp.png"));
    free(url);
    url = ju_url_next(iter);
    assert(0 == strcmp(url, "https://www.archlinux.org/static/logos/archlinux-logo-dark-90dpi.ebdee92a15b3.png"));
    free(url);
    url = ju_url_next(iter);
    assert(0 == strcmp(url, "https://i.reddituploads.com/d1e77b5c62694624ba7235a57431f070?fit=max&h=1536&w=1536&s=b3103272b2bf369f5c42396b09c4caf8"));
    free(url);
    printf("Passed: url iteration\n");
}

void test_sample_data(void) {
    ju_json_t *json = test_parse();
    test_object_get(json);
    struct ju_array_iter *iter = test_array_init(json);
    test_array_next(iter);
    free(iter);
    struct ju_random_iter *r_iter = test_random_init(json);
    test_random_next(r_iter);
    ju_random_free(r_iter);
    ju_free(json);
}

void test_url_data(void) {
    char *data = "{\"data\": {\"children\": [{\"data\": {\"url\": \"https://www.google.com/images/branding/googlelogo/1x/googlelogo_color_272x92dp.png\"}}, {\"data\": {\"url\": \"https://www.archlinux.org/static/logos/archlinux-logo-dark-90dpi.ebdee92a15b3.png\"}}, {\"data\": {\"url\": \"https://i.reddituploads.com/d1e77b5c62694624ba7235a57431f070?fit=max&h=1536&w=1536&s=b3103272b2bf369f5c42396b09c4caf8\"}}]}}";
    ju_json_t *json = ju_parse(data);
    struct ju_array_iter *iter = test_url_init(json);
    test_url_next(iter);
    ju_free(json);
    free(iter);
}

int ju_test_main(void) {
    test_sample_data();
    test_url_data();
    return 0;
}

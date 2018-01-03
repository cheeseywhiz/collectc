#ifndef RANDOM_POPPER_H
#define RANDOM_POPPER_H

#include <unistd.h>

typedef struct rp_node {
    void *data;
    struct rp_node *next;
} rp_t;

typedef void (*rp_free_func)(void *ptr);

rp_t* rp_new(void *data);
void rp_shallow_free(rp_t **self);
void rp_deep_free(rp_t **self, rp_free_func free_data);
rp_t* rp_second_to_last(rp_t **self);
rp_t* rp_last(rp_t **self);
rp_t* rp_append(rp_t **self, void *data);
rp_t* rp_get_index(rp_t **self, size_t index);
size_t rp_len(rp_t **self);
void* rp_pop_index(rp_t **self, size_t index);
void* rp_pop_random(rp_t **self);

int* new_int(int value);

#endif

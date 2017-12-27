#ifndef RANDOM_POPPER_H
#define RANDOM_POPPER_H

#include <unistd.h>

typedef struct rp_node {
    int num;
    struct rp_node *next;
} rp_t;

rp_t* rp_new(int num);
void rp_free(rp_t **self);
rp_t* rp_second_to_last(rp_t **self);
rp_t* rp_last(rp_t **self);
rp_t* rp_append(rp_t **self, int num);
rp_t* rp_get_index(rp_t **self, size_t index);
size_t rp_len(rp_t **self);
int rp_pop_index(rp_t **self, size_t index);
int rp_pop_random(rp_t **self);
rp_t* rp_init(int length);

#endif

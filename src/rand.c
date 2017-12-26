#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include "rand.h"

unsigned int urandom_number(void) {
    unsigned int buf[sizeof(unsigned int)];
    int urandom = open("/dev/urandom", O_RDONLY);
    int read_size = read(urandom, buf, sizeof(unsigned int));
    int close_code = close(urandom);

    if (read_size != sizeof(unsigned int) || close_code != 0) {
        return 0;
    } else {
        return *buf;
    }
}

int randbelow(int n) {
    return floor(n * ((double) rand() / RAND_MAX));
}

static int items_contains_num(int *items, int items_length, int num) {
    for (int i = 0; i < items_length; i++) {
        if (num == items[i]) {
            return 1;
        }
    }

    return 0;
}

static int new_unique_rand_below(int *items, int items_length, int random_max) {
    int i = 0;
    int num = randbelow(random_max);

    for (; i < items_length && items_contains_num(items, items_length, num);) {
        i++;
        num = randbelow(random_max);
    }

    return num;
}

int_list* int_list_random_order(int max) {
    srand(urandom_number());
    int_list *self = malloc(sizeof(int_list));

    if (!self) {
        return NULL;
    }

    self->length = max;
    self->items = calloc(self->length, sizeof(int));

    if (!self->items) {
        free(self);
        return NULL;
    }

    for (int i = 0; i < self->length; i++) {
        self->items[i] = new_unique_rand_below(self->items, i, max);
    }

    return self;
}

void free_int_list(int_list *self) {
    free(self->items);
    free(self);
}

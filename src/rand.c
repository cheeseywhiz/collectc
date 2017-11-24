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
    read(urandom, buf, sizeof(unsigned int));
    close(urandom);
    return *buf;
}

static int int_list_contains(int_list *self, int num) {
    for (int i = 0; i < self->length; i++) {
        if (num == self->items[i]) {
            return 1;
        }
    }

    return 0;
}

int randbelow(int n) {
    return floor(n * ((double) random() / RAND_MAX));
}

int_list* int_list_random_order(int max) {
    srandom(urandom_number());
    int i, j;
    int_list *self = malloc(sizeof(int_list));

    if (!self) {
        fprintf(stderr, "int_list_random_order self malloc failed\n");
        return NULL;
    }

    self->items = NULL;
    self->length = 0;

    for (i = 0; i < max; i++) {
        for (j = randbelow(max); int_list_contains(self, j); j = randbelow(max));
        int *ptr = realloc(self->items, sizeof(int) * (self->length + 1));

        if (ptr) {
            self->items = ptr;
        } else {
            fprintf(stderr, "int_list_append ptr realloc failed\n");
            return NULL;
        }

        self->items[self->length++] = j;
    }

    return self;
}

void free_int_list(int_list *self) {
    free(self->items);
    free(self);
}

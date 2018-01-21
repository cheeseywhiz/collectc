#include <stdlib.h>

#include "log.h"
#include "random_popper.h"
#include "rand.h"

rp_t* rp_new(void *data) {
    rp_t *self = malloc(sizeof(rp_t));

    if (!self) {
        LOG_ERRNO();
        return NULL;
    }

    self->data = data;
    self->next = NULL;
    self->prev = NULL;
    return self;
}

void rp_deep_free(rp_t **self, rp_free_func free_data) {
    rp_t *item, *item_alias;

    for (item = *self; item;) {
        item_alias = item;
        item = item->next;
        free_data(item_alias->data);
        free(item_alias);
    }
}

static void no_op_free(__attribute__((unused)) void *ptr) {};

void rp_shallow_free(rp_t **self) {
    rp_deep_free(self, no_op_free);
}

rp_t* rp_last(rp_t **self) {
    if (!*self) {
        return NULL;
    }

    rp_t *last;
    for (last = *self; last->next; last = last->next);
    return last;
}

rp_t* rp_append(rp_t **self, void *data) {
    rp_t *new = rp_new(data);

    if (!new) {
        return NULL;
    } else if (*self) {
        rp_t *last = rp_last(self);
        last->next = new;
        new->prev = last;
    } else {
        *self = new;
    }

    return new;
}

rp_t* rp_get_index(rp_t **self, size_t index) {
    if (!*self) {
        return NULL;
    } else if (index >= rp_len(self)) {
        EXCEPTION("index out of bounds");
        return NULL;
    }

    rp_t *item = *self;

    for (size_t i = 0; i < index; i++) {
        if (!item) {
            EXCEPTION("rp_t->item is unexpectedly NULL");
            return NULL;
        }

        item = item->next;
    }

    return item;
}

size_t rp_len(rp_t **self) {
    if (!*self) {
        return 0;
    }

    size_t i = 0;
    for (rp_t *item = *self; item; item = item->next) i++;
    return i;
}

void* rp_pop_index(rp_t **self, size_t index) {
    rp_t *item;
    size_t self_len = rp_len(self);

    if (!self_len) {
        if (index) EXCEPTION("index out of bounds");
        item = NULL;
    } else if (index >= self_len) {
        EXCEPTION("index out of bounds");
        item = NULL;
    } else if (!index || self_len == 1) {
        item = *self;
        *self = item->next;
    } else {
        item = rp_get_index(self, index);
        if (!item) return NULL;

        rp_t *before, *after;
        before = item->prev;
        after = item->next;

        if (before) before->next = after;
        if (after) after->prev = before;
    }

    void *data;

    if (!item) {
        data = NULL;
    } else {
        data = item->data;
        free(item);
    }

    return data;
}

void* rp_pop_random(rp_t **self) {
    return rp_pop_index(self, randbelow(rp_len(self)));
}

int* new_int(int value) {
    int *new = malloc(sizeof(int));

    if (!new) {
        LOG_ERRNO();
        return NULL;
    }

    *new = value;
    return new;
}

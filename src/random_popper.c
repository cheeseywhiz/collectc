#include <stdlib.h>

#include "random_popper.h"
#include "rand.h"

rp_t* rp_new(void *data) {
    rp_t *self = malloc(sizeof(rp_t));

    if (!self) {
        return NULL;
    }

    self->data = data;
    self->next = NULL;
    return self;
}

void rp_shallow_free(rp_t **self) {
    rp_t *item, *item_alias;

    for (item = *self; item;) {
        item_alias = item;
        item = item->next;
        free(item_alias);
    }
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

rp_t* rp_second_to_last(rp_t **self) {
    if (!*self) {
        return NULL;
    }

    rp_t *second_to_last = NULL;

    for (rp_t *last = *self; last->next; last = last->next) {
        second_to_last = last;
    }

    return second_to_last;
}

rp_t* rp_last(rp_t **self) {
    if (!*self) {
        return NULL;
    }

    rp_t *second_to_last = rp_second_to_last(self);

    if (second_to_last) {
        return second_to_last->next;
    } else {
        return *self;
    }
}

rp_t* rp_append(rp_t **self, void *data) {
    rp_t *new = rp_new(data);

    if (!new) {
        return NULL;
    }

    if (*self) {
        rp_last(self)->next = new;
    } else {
        *self = new;
    }

    return new;
}

rp_t* rp_get_index(rp_t **self, size_t index) {
    if (!*self) {
        return NULL;
    } else if (index >= rp_len(self)) {
        return NULL;
    }

    rp_t *item = *self;

    for (size_t i = 0; i < index; i++) {
        if (!item) {
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

    if (index >= self_len || !self_len) {
        item = NULL;
    } else if (self_len == 1) {
        item = *self;
        *self = NULL;
    } else if (!index) {
        item = *self;
        *self = item->next;
    } else {
        rp_t *before = rp_get_index(self, index - 1);
        item = before->next;
        before->next = item->next;
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
        return NULL;
    }

    *new = value;
    return new;
}

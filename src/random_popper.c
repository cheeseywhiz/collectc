#include <stdlib.h>

#include "random_popper.h"
#include "rand.h"

rp_t* rp_new(int num) {
    rp_t *self = malloc(sizeof(rp_t));

    if (!self) {
        return NULL;
    }

    self->num = num;
    self->next = NULL;
    return self;
}

void rp_free(rp_t **self) {
    rp_t *item, *item_alias;

    for (item = *self; item;) {
        item_alias = item;
        item = item->next;
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

rp_t* rp_append(rp_t **self, int num) {
    rp_t *new = rp_new(num);

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

int rp_pop_index(rp_t **self, size_t index) {
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

    int num;

    if (!item) {
        num = -1;
    } else {
        num = item->num;
        free(item);
    }

    return num;
}

int rp_pop_random(rp_t **self) {
    return rp_pop_index(self, randbelow(rp_len(self)));
}

rp_t* rp_init(int length) {
    srand(urandom_number());
    rp_t *self = NULL;

    for (int i = 0; i < length; i++) {
        if (!rp_append(&self, i)) {
            break;
        }
    }

    return self;
}

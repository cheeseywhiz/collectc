#ifndef RAND_H
#define RAND_H

void rand_reseed(void);

typedef struct {
    int *items;
    int length;
} int_list;

int randbelow(int n);

#endif

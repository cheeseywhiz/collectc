#ifndef RAND_H
#define RAND_H

#include <limits.h>
#define URAND_MAX UINT_MAX
unsigned int urandom_number(void);

typedef struct {
    int *items;
    int length;
} int_list;

int randbelow(int n);

#endif

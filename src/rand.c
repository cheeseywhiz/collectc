#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "rand.h"

void rand_reseed(void) {
    srand(time(NULL));
}

int randbelow(int n) {
    return floor(n * ((double) rand() / RAND_MAX));
}

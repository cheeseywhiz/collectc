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

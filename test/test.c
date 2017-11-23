#include "get.h"

#include "iter.c"

int exit_code;
#define EXIT(x) exit_code = x; if(exit_code) return exit_code;

int main(void) {
    EXIT(iter())

    return 0;
}

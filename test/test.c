#include "iter.c"
#include "randtest.c"

int exit_code;
#define EXIT(x) exit_code = x; if(exit_code) return exit_code;

int main(void) {
    EXIT(iter())
    EXIT(randtest())

    return 0;
}

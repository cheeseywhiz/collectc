#include "randtest.c"

int exit_code;
#define EXIT(x) exit_code = x; if(exit_code) return exit_code;

#include "jsmntest.c"

int main(void) {
    EXIT(randtest())
    EXIT(ju_test_main())
    return 0;
}

#include "jsmntest.c"
#include "randtest.c"
#include "regtest.c"

int exit_code;
#define EXIT(x) exit_code = x; if(exit_code) return exit_code;

int main(void) {
    EXIT(randtest())
    EXIT(ju_test_main())
    EXIT(regex_test_main())
    return 0;
}

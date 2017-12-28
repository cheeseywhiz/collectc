#include <stdio.h>

#include "test.h"

int main(void) {
    MAIN_INIT();
    MODULE_REPORT("rand.c", rand_test_main());
    MODULE_REPORT("jsmnutils.c", ju_test_main());
    MODULE_REPORT("reg.c", regex_test_main());
    MODULE_REPORT("path.c", path_test_main());
    MODULE_REPORT("random_popper.c", rp_test_main());
    MAIN_EXIT();
}

#include "rand.h"
#include "log.h"
#include "test.h"

BIG_TEST rand_test_main(void);
BIG_TEST ju_test_main(void);
BIG_TEST regex_test_main(void);
BIG_TEST path_test_main(void);
BIG_TEST rp_test_main(void);
BIG_TEST raw_test_main(void);
BIG_TEST auth_test_main(void);
BIG_TEST ini_test_main(void);

int main(int argc, char *argv[]) {
    if (argc) log_init_prog_name(argv);
    SET_LOG_LEVEL(LOG_WARNING);
    rand_reseed();
    SCORE_INIT();
    BIG_REPORT("rand.c", rand_test_main());
    BIG_REPORT("jsmnutils.c", ju_test_main());
    BIG_REPORT("reg.c", regex_test_main());
    BIG_REPORT("path.c", path_test_main());
    BIG_REPORT("random_popper.c", rp_test_main());
    BIG_REPORT("raw.c", raw_test_main());
    BIG_REPORT("auth.c", auth_test_main());
    BIG_REPORT("ini.c", ini_test_main());
    MAIN_EXIT();
}

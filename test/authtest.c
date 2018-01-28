#include <stdio.h>

#include "log.h"
#include "test.h"
#include "auth.h"
#include "path.h"

SMALL_TEST test_config_path(void) {
    SCORE_INIT();
    path_touch("praw.ini");
    char *config_path = auth_config_path();

    if (!config_path) {
        EXITFAIL(1);
    }

    ASSERT(path_exists(config_path));
    free(config_path);

    if (remove("praw.ini")) {
        EXITFAIL(1);
    }

    RETURN_SCORE();
}

BIG_TEST auth_test_main(void) {
    SCORE_INIT();
    SMALL_REPORT("auth_config_path()", test_config_path());
    RETURN_SCORE();
}

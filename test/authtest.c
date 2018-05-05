#include <stdio.h>
#include <string.h>

#include "log.h"
#include "test.h"
#include "auth.h"
#include "path.h"

SMALL_TEST test_config_path(void) {
    SCORE_INIT();
    char *config_path = auth_config_path();

    if (!config_path) {
        EXITFAIL(1);
    }

    ASSERT(path_exists(config_path));
    free(config_path);
    RETURN_SCORE();
}

SMALL_TEST test_get_access_token(void) {
    SCORE_INIT();
    char *tok = auth_get_default_access_token();
    ASSERT(tok);
    free(tok);
    RETURN_SCORE();
}

BIG_TEST auth_test_main(void) {
    SCORE_INIT();
    SMALL_REPORT("auth_config_path()", test_config_path());
    SMALL_REPORT("auth_get_access_token()", test_get_access_token());
    RETURN_SCORE();
}

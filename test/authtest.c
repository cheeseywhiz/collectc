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

struct case_init_default_profile {
    char *expected;
    char *actual;
};

SMALL_TEST test_init_default_profile(void) {
    SCORE_INIT();
    int n_cases = 4;
    struct auth_profile profile;

    if (auth_init_default_profile(&profile)) {
        EXITFAIL(n_cases);
    }

    struct case_init_default_profile cases[] = {
        {"cheeseywhiz", profile.username},
        {"hunter2", profile.password},
        {"XxXclientIDXxX", profile.client_id},
        {"*******", profile.secret}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_init_default_profile case_ = cases[i];
        ASSERT(!strcmp(case_.expected, case_.actual));
    }

    auth_free_profile(&profile);
    RETURN_SCORE();
}

BIG_TEST auth_test_main(void) {
    SCORE_INIT();
    SMALL_REPORT("auth_config_path()", test_config_path());
    SMALL_REPORT("auth_init_default_profile()", test_init_default_profile());
    RETURN_SCORE();
}

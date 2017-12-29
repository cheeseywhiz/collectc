#ifndef TEST_H
#define TEST_H

#include <unistd.h>

struct score {
    size_t passing;
    size_t failing;
};

#define TEST_MOD struct score
#define TEST_CASE static struct score

#define SCORE(pass, fail) \
    (struct score) { \
        .passing = pass, \
        .failing = fail, \
    }

#define NEW_SCORE() SCORE(0, 0)

#define SCORE_INIT() struct score subscore, score_local = NEW_SCORE()
#define PASSING score_local.passing
#define FAILING score_local.failing
#define PASS() PASSING++
#define FAIL() FAILING++
#define ASSERT(bool) if (bool) PASS(); else FAIL()
#define ADD_SCORE(pass, fail) \
    PASSING += pass; \
    FAILING += fail
#define SUBSCORE(score) \
    subscore = score; \
    ADD_SCORE(subscore.passing, subscore.failing)
#define RETURN_SCORE() return score_local
#define EXITFAIL(n_cases) \
    ADD_SCORE(0, n_cases - PASSING); \
    RETURN_SCORE()

#define REPORT_STR "%s:\nPass: %ld\nFail: %ld\n"
#define FUNCTION_REPORT(name, score) \
    SUBSCORE(score); \
    printf(REPORT_STR, name, subscore.passing, subscore.failing)

#define MODULE_REPORT_STR "%s summary:\nPass: %ld\nFail: %ld\n\n"
#define MODULE_REPORT(name, score) \
    printf("%s:\n", name); \
    SUBSCORE(score); \
    printf(MODULE_REPORT_STR, name, subscore.passing, subscore.failing)

#define MAIN_EXIT() \
    printf(REPORT_STR, "test summary", PASSING, FAILING); \
    return FAILING

TEST_MOD rand_test_main(void);
TEST_MOD ju_test_main(void);
TEST_MOD regex_test_main(void);
TEST_MOD path_test_main(void);
TEST_MOD rp_test_main(void);

#endif

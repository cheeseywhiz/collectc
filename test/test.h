#ifndef TEST_H
#define TEST_H

#include <stdio.h>

struct score {
    size_t passing;
    size_t failing;
};

#define BIG_TEST struct score
#define SMALL_TEST static struct score

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
#define ADD_PASS(n) PASSING += n
#define ADD_FAIL(n) FAILING += n
#define ADD_SCORE(pass, fail) \
    ADD_PASS(pass); \
    ADD_FAIL(fail)
#define SUBSCORE(score) \
    subscore = score; \
    ADD_SCORE(subscore.passing, subscore.failing)
#define RETURN_SCORE() return score_local
#define EXITFAIL(n_cases) \
    ADD_SCORE(0, n_cases - PASSING); \
    RETURN_SCORE()

#define REPORT_SCORE(passing, failing) printf("Pass: %ld\nFail: %ld\n", passing, failing)
#define REPORT_SUBSCORE() REPORT_SCORE(subscore.passing, subscore.failing)
#define REPORT_CURRENT() REPORT_SCORE(PASSING, FAILING)

#define SMALL_REPORT(name, score) \
    printf("%s:\n", name); \
    SUBSCORE(score); \
    REPORT_SUBSCORE()

#define BIG_REPORT(name, score) \
    printf("%s:\n", name); \
    SUBSCORE(score); \
    printf("%s summary:\n", name); \
    REPORT_SUBSCORE(); \
    printf("\n")

#define MAIN_EXIT() \
    printf("test summary:\n"); \
    REPORT_CURRENT(); \
    return FAILING

#endif

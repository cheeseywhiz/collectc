#ifndef TEST_H
#define TEST_H

#include <unistd.h>

struct score {
    size_t passing;
    size_t failing;
};

#define SCORE(pass, fail) \
    (struct score) { \
        .passing = pass, \
        .failing = fail \
    }

#define NEW_SCORE() SCORE(0, 0)

#define SCORE_INIT() struct score score_local = NEW_SCORE()
#define PASS() score_local.passing++
#define FAIL() score_local.failing++
#define RETURN_SCORE() return score_local
#define ASSERT(bool) if (bool) PASS(); else FAIL()

#define SSSCORE static struct score
#define REPORT_STR "%s:\nPass: %ld\nFail: %ld\n%s"

#define MODULE_INIT() struct score function_score, module_score = NEW_SCORE()

#define FUNCTION_REPORT(name, score) \
    function_score = score; \
    module_score.passing += function_score.passing; \
    module_score.failing += function_score.failing; \
    printf(REPORT_STR, name, function_score.passing, function_score.failing, "")

#define MODULE_EXIT() return module_score

#define MAIN_INIT() struct score module_score, main_score = NEW_SCORE()

#define MODULE_REPORT(name, score) \
    printf("%s:\n", name); \
    module_score = score; \
    main_score.passing += module_score.passing; \
    main_score.failing += module_score.failing; \
    printf(REPORT_STR, "This module", module_score.passing, module_score.failing, "\n")

#define MAIN_EXIT() \
    printf(REPORT_STR, "All tests", main_score.passing, main_score.failing, ""); \
    return main_score.failing

struct score rand_test_main(void);
struct score ju_test_main(void);
struct score regex_test_main(void);
struct score path_test_main(void);
struct score rp_test_main(void);

#endif

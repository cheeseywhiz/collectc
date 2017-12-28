#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "reg.h"
#include "test.h"

struct case_starts_with {
    char *haystack;
    char *needle;
    int expected;
};

static struct score test_starts_with(void) {
    SCORE_INIT();
    int n_cases = 4;
    struct case_starts_with cases[] = {
        {"apple", "apple", 1},
        {"apple", "orange", 0},
        {"apple", "apples", 0},
        {"oranges", "orange", 1}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_starts_with case_ = cases[i];
        int actual = regex_starts_with(case_.haystack, case_.needle);
        ASSERT(actual == case_.expected);
    }

    RETURN_SCORE();
}

static struct score test_ends_with(void) {
    SCORE_INIT();
    int n_cases = 5;
    struct case_starts_with cases[] = {
        {"apple", "apple", 1},
        {"apple", "orange", 0},
        {"apple", "ple", 1},
        {"honeycrisp apple", "apple", 1},
        {"orange", "apple", 0}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_starts_with case_ = cases[i];
        int actual = regex_ends_with(case_.haystack, case_.needle);
        ASSERT(actual == case_.expected);
    }

    RETURN_SCORE();
}

struct case_match_one_subexpr {
    char *haystack;
    char *expected;
};

static struct score test_match_one_subexpr(void) {
    SCORE_INIT();
    int n_cases = 4;
    char *pattern = "\\([0-9]{3}\\)-[0-9]{3}-([0-9]{4})";
    struct case_match_one_subexpr cases[] = {
        {"(012)-128-1361", "1361"},
        {"(459)-103-1916", "1916"},
        {"(715)-181-1174", "1174"},
        {"(946)-633-9631", "9631"}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_match_one_subexpr case_ = cases[i];
        char *actual = regex_match_one_subexpr(pattern, case_.haystack, REG_EXTENDED);
        ASSERT(strcmp(actual, case_.expected) == 0);
        free(actual);
    }

    RETURN_SCORE();
}

struct case_contains {
    char *needle;
    char *haystack;
    int expected;
};

static struct score test_contains(void) {
    SCORE_INIT();
    int n_cases = 5;
    struct case_contains cases[] = {
        {"removed", "http://i.imgur.com/removed.jpg", 1},
        {"image", "image/png", 1},
        {"apple", "orange", 0},
        {"house", "tree", 0},
        {"tree", "treehouse", 1}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_contains case_ = cases[i];
        int actual = regex_contains(case_.needle, case_.haystack);
        ASSERT(actual == case_.expected);
    }

    RETURN_SCORE();
}

struct case_str_slice {
    int start;
    int end;
    char *expected;
};

static struct score test_str_slice(void) {
    SCORE_INIT();
    int n_cases = 7;
    char *string = "hello world";
    struct case_str_slice cases[] = {
        {6, 5, NULL},
        {-1, 5, NULL},
        {11, 12, NULL},
        {5, -1, NULL},
        {5, 12, NULL},
        {5, 5, ""},
        {6, 11, "world"}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_str_slice case_ = cases[i];
        char *actual = regex_str_slice(string, case_.start, case_.end);
        int cmp_eq = 0;

        if (actual) {
            cmp_eq = strcmp(case_.expected, actual) == 0;
            free(actual);
        } else if (!case_.expected) {
            cmp_eq = 1;
        }

        ASSERT(cmp_eq);
    }

    RETURN_SCORE();
}

struct case_url_fname {
    char *url;
    char *expected;
};

static struct score test_url_fname(void) {
    SCORE_INIT();
    int n_cases = 4;
    struct case_url_fname cases[] = {
        {"http://www.google.com/image.jpg", "image.jpg"},
        {"https://www.reddit.com/r/all/whatever.png", "whatever.png"},
        {"ftp://i.imgur.com/dir1/dir2/dir3/abc123?query", "abc123"},
        {"https://imgur.com/a/album#fragment", "album"}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_url_fname case_ = cases[i];
        char *actual = regex_url_fname(case_.url);
        int cmp_eq = strcmp(case_.expected, actual) == 0;

        if (actual) {
            free(actual);
        }

        ASSERT(cmp_eq);
    }

    RETURN_SCORE();
}

struct score regex_test_main(void) {
    MODULE_INIT();
    FUNCTION_REPORT("regex_starts_with()", test_starts_with());
    FUNCTION_REPORT("regex_ends_with()", test_ends_with());
    FUNCTION_REPORT("regex_match_one_subexpr()", test_match_one_subexpr());
    FUNCTION_REPORT("regex_contains()", test_contains());
    FUNCTION_REPORT("regex_str_slice()", test_str_slice());
    FUNCTION_REPORT("regex_url_fname()", test_url_fname());
    MODULE_EXIT();
}

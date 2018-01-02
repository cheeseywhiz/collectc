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

SMALL_TEST test_starts_with(void) {
    SCORE_INIT();
    int n_cases = 4;
    struct case_starts_with cases[] = {
        {"apple", "apple", 1},
        {"apple", "orange", 0},
        {"apple", "apples", 0},
        {"oranges", "orange", 1},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_starts_with case_ = cases[i];
        int actual = regex_starts_with(case_.haystack, case_.needle);
        ASSERT(actual == case_.expected);
    }

    RETURN_SCORE();
}

SMALL_TEST test_ends_with(void) {
    SCORE_INIT();
    int n_cases = 5;
    struct case_starts_with cases[] = {
        {"apple", "apple", 1},
        {"apple", "orange", 0},
        {"apple", "ple", 1},
        {"honeycrisp apple", "apple", 1},
        {"orange", "apple", 0},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_starts_with case_ = cases[i];
        int actual = regex_ends_with(case_.haystack, case_.needle);
        ASSERT(actual == case_.expected);
    }

    RETURN_SCORE();
}

struct case_find {
    char *haystack;
    char *needle;
    size_t expected;
};

SMALL_TEST test_find(void) {
    SCORE_INIT();
    int n_cases = 3;
    struct case_find cases[] = {
        {"hello world", "hello", 0},
        {"hello world", "world", 6},
        {"tree house", "house", 5},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_find case_ = cases[i];
        size_t actual = regex_find(case_.haystack, case_.needle);
        ASSERT(case_.expected == actual);
    }

    RETURN_SCORE();
}

struct case_remove {
    char *haystack;
    char *needle;
    char *expected;
};

SMALL_TEST test_remove(void) {
    SCORE_INIT();
    int n_cases = 4;
    struct case_remove cases[] = {
        {"hello world", "world", "hello "},
        {"treehouse", "house", "tree"},
        {"the quick brown fox jumps over the lazy dog", "the ", "quick brown fox jumps over the lazy dog"},
        {"abracadabra", "bra", "acadabra"},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_remove case_ = cases[i];
        char *actual = regex_remove(case_.haystack, case_.needle);
        ASSERT(!strcmp(case_.expected, actual));
        free(actual);
    }

    RETURN_SCORE();
}

struct case_remove_first_pattern {
    char *haystack;
    char *expected;
};

SMALL_TEST test_remove_first_pattern(void) {
    SCORE_INIT();
    char *pattern = "([^/]*/\\.\\./?)";
    int n_cases = 3;
    struct case_remove_first_pattern cases[] = {
        {"dir/file", "dir/file"},
        {"dir/..", ""},
        {"dir1/dir2/../..", "dir1/.."},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_remove_first_pattern case_ = cases[i];
        char *actual = regex_remove_first_pattern(case_.haystack, pattern, 0);
        ASSERT(!strcmp(case_.expected, actual));
        free(actual);
    }

    RETURN_SCORE();
}

struct case_match {
    char *haystack;
    char *expected;
};

SMALL_TEST test_match_one_subexpr(void) {
    SCORE_INIT();
    int n_cases = 4;
    char *pattern = "\\([0-9]{3}\\)-[0-9]{3}-([0-9]{4})";
    struct case_match cases[] = {
        {"(012)-128-1361", "1361"},
        {"(459)-103-1916", "1916"},
        {"(715)-181-1174", "1174"},
        {"(946)-633-9631", "9631"},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_match case_ = cases[i];
        char *actual = regex_match_one_subexpr(pattern, case_.haystack, REG_EXTENDED);
        ASSERT(strcmp(actual, case_.expected) == 0);
        free(actual);
    }

    RETURN_SCORE();
}

struct case_contains {
    char *haystack;
    char *needle;
    int expected;
};

SMALL_TEST test_contains(void) {
    SCORE_INIT();
    int n_cases = 5;
    struct case_contains cases[] = {
        {"http://i.imgur.com/removed.jpg", "removed", 1},
        {"image/png", "image", 1},
        {"orange", "apple", 0},
        {"tree", "house", 0},
        {"treehouse", "tree", 1},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_contains case_ = cases[i];
        int actual = regex_contains(case_.haystack, case_.needle);
        ASSERT(actual == case_.expected);
    }

    RETURN_SCORE();
}

struct case_str_slice {
    int start;
    int end;
    char *expected;
};

SMALL_TEST test_str_slice(void) {
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
        {6, 11, "world"},
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

SMALL_TEST test_url_fname(void) {
    SCORE_INIT();
    int n_cases = 4;
    struct case_url_fname cases[] = {
        {"http://www.google.com/image.jpg", "image.jpg"},
        {"https://www.reddit.com/r/all/whatever.png", "whatever.png"},
        {"ftp://i.imgur.com/dir1/dir2/dir3/abc123?query", "abc123"},
        {"https://imgur.com/a/album#fragment", "album"},
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

BIG_TEST regex_test_main(void) {
    SCORE_INIT();
    SMALL_REPORT("regex_starts_with()", test_starts_with());
    SMALL_REPORT("regex_ends_with()", test_ends_with());
    SMALL_REPORT("regex_str_slice()", test_str_slice());
    SMALL_REPORT("regex_find()", test_find());
    SMALL_REPORT("regex_remove()", test_remove());
    SMALL_REPORT("regex_remove_first_pattern()", test_remove_first_pattern());
    SMALL_REPORT("regex_match_one_subexpr()", test_match_one_subexpr());
    SMALL_REPORT("regex_contains()", test_contains());
    SMALL_REPORT("regex_url_fname()", test_url_fname());
    RETURN_SCORE();
}

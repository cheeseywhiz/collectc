#include <stdio.h>
#include <string.h>
#include <regex.h>
#include "reg.h"

struct score {
    int passing;
    int failing;
};

struct case_starts_with {
    char *haystack;
    char *needle;
    int expected;
};

struct score test_starts_with(void) {
    struct score score_starts_with = {0, 0};
    int n_cases = 4;
    struct case_starts_with cases[4] = {
        {"apple", "apple", 1},
        {"apple", "orange", 0},
        {"apple", "apples", 0},
        {"oranges", "orange", 1}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_starts_with case_ = cases[i];
        int actual = regex_starts_with(case_.haystack, case_.needle);
        if (actual == case_.expected) {
            score_starts_with.passing++;
        } else {
            score_starts_with.failing++;
        }
    }

    return score_starts_with;
}

struct case_match_one_subexpr {
    char *haystack;
    char *expected;
};

struct score test_match_one_subexpr(void) {
    struct score score_match_one_subexpr = {0, 0};
    int n_cases = 4;
    char *pattern = "\\([0-9]{03}\\)-[0-9]{3}-([0-9]{4})";
    struct case_match_one_subexpr cases[4] = {
        {"(012)-128-1361", "1361"},
        {"(459)-103-1916", "1916"},
        {"(715)-181-1174", "1174"},
        {"(946)-633-9631", "9631"}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_match_one_subexpr case_ = cases[i];
        char *actual = regex_match_one_subexpr(pattern, case_.haystack, REG_EXTENDED);

        if (strcmp(actual, case_.expected) == 0) {
            score_match_one_subexpr.passing++;
        } else {
            score_match_one_subexpr.failing++;
        }

        free(actual);
    }

    return score_match_one_subexpr;
}

struct case_str_slice {
    int start;
    int end;
    char *expected;
};

struct score test_str_slice(void) {
    struct score score_str_slice = {0, 0};
    int n_cases = 7;
    char *string = "hello world";
    struct case_str_slice cases[7] = {
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

        if (case_.expected == NULL && actual == NULL) {
            score_str_slice.passing++;
        } else if (strcmp(case_.expected, actual) == 0) {
            free(actual);
            score_str_slice.passing++;
        } else {
            score_str_slice.failing++;
        }
    }

    return score_str_slice;
}

struct case_url_fname {
    char *url;
    char *expected;
};

struct score test_url_fname(void) {
    struct score score_url_fname = {0, 0};
    int n_cases = 4;
    struct case_url_fname cases[4] = {
        {"http://www.google.com/image.jpg", "image.jpg"},
        {"https://www.reddit.com/r/all/whatever.png", "whatever.png"},
        {"ftp://i.imgur.com/dir1/dir2/dir3/abc123?query", "abc123"},
        {"https://imgur.com/a/album#fragment", "album"}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_url_fname case_ = cases[i];
        char *actual = regex_url_fname(case_.url);

        if (strcmp(case_.expected, actual) == 0) {
            free(actual);
            score_url_fname.passing++;
        } else {
            score_url_fname.failing++;
        }
    }

    return score_url_fname;
}

#define REPORT(score) printf("Passing: %d\nFailing: %d\n", score.passing, score.failing);

int regex_test_main(void) {
    printf("regex_starts_with():\n");
    REPORT(test_starts_with());
    printf("regex_match_one_subexpr():\n");
    REPORT(test_match_one_subexpr());
    printf("regex_str_slice():\n");
    REPORT(test_str_slice())
    printf("regex_url_fname():\n");
    REPORT(test_url_fname())
    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#include "path.h"
#include "test.h"

struct case_norm {
    char *input;
    char *expected;
};

SSSCORE test_norm(void) {
    SCORE_INIT();
    int n_cases = 8;
    struct case_norm cases[] = {
        {"~root", "/root"},
        {"~root/dir/file", "/root/dir/file"},
        {"~unknownuser", "~unknownuser"},
        {"./", "."},
        {".", "."},
        {"./cmd", "cmd"},
        {"dir/", "dir"},
        {"./both/", "both"}
    };
    char *actual;

    for (int i = 0; i < n_cases; i++) {
        struct case_norm case_ = cases[i];
        actual = path_norm(case_.input);
        ASSERT(path_eq(case_.expected, actual));
        free(actual);
    }

    RETURN_SCORE();
}

SSSCORE test_exists_true(void) {
    SCORE_INIT();
    int n_cases = 6;
    char *cwd = path_cwd();
    char *cases[] = {
        "/",
        cwd,
        "/tmp",
        "~",
        "~root",
        "/dev"
    };

    for (int i = 0; i < n_cases; i++) {
        char *case_ = cases[i];
        ASSERT(path_exists(case_));
    }

    free(cwd);
    RETURN_SCORE();
}

SSSCORE test_exists_false(void) {
    SCORE_INIT();
    int n_cases = 3;
    char *cwd = path_cwd();
    char *dne = path_join(cwd, "does_not_exist.txt");
    char *cases[] = {
        "/does/not/exist",
        ".file123helloworld",
        "~root/does/not/exist",
        dne
    };

    for (int i = 0; i < n_cases; i++) {
        char *case_ = cases[i];
        ASSERT(!path_exists(case_));
    }

    free(dne);
    free(cwd);
    RETURN_SCORE();
}

SSSCORE test_is_abs_true(void) {
    SCORE_INIT();
    int n_cases = 4;
    char *cases[] = {
        "~",
        "/",
        "/home",
        "~root"
    };

    for (int i = 0; i < n_cases; i++) {
        char *case_ = cases[i];
        ASSERT(path_is_abs(case_));
    }

    RETURN_SCORE();
}

SSSCORE test_is_abs_false(void) {
    SCORE_INIT();
    int n_cases = 3;
    char *cases[] = {
        ".",
        "dir",
        "../dir/file"
    };

    for (int i = 0; i < n_cases; i++) {
        char *case_ = cases[i];
        ASSERT(!path_is_abs(case_));
    }

    RETURN_SCORE();
}

struct case_url_fname {
    char *path;
    char *url;
    char *expected;
};

SSSCORE test_url_fname(void) {
    SCORE_INIT();
    int n_cases = 3;
    struct case_url_fname cases[] = {
        {"~root", "http://www.reddit.com/subreddits", "/root/subreddits"},
        {"/", "https://i.imgur.com/abcdef.png", "/abcdef.png"},
        {".", "ftp://github.com/README.md", "README.md"}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_url_fname case_ = cases[i];
        char *actual = path_url_fname(case_.path, case_.url);
        ASSERT(path_eq(case_.expected, actual));
        free(actual);
    }

    RETURN_SCORE();
}

struct case_join {
    char *path;
    char *other;
    char *expected;
};

SSSCORE test_join(void) {
    SCORE_INIT();
    int n_cases = 7;
    struct case_join cases[] = {
        {".", "dir", "dir"},
        {"dir", "file", "dir/file"},
        {"/", "dev", "/dev"},
        {"tmp", "/home", "/home"},
        {"/tmp/dir", "", "/tmp/dir"},
        {"/tmp", "./..", "/"},
        {"/tmp", "./", "/tmp"}
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_join case_ = cases[i];
        char *actual = path_join(case_.path, case_.other);
        ASSERT(path_eq(case_.expected, actual));
        free(actual);
    }

    printf("\n");
    RETURN_SCORE();
}

SSSCORE test_mkdir(char *prefix) {
    SCORE_INIT();
    int n_cases = 4;
    char *dir = "dir";
    char *joined = path_join(dir, "dir2");
    char *cases[] = {
        dir,
        joined,
        "dir3/dir4",
        dir
    };

    for (int i = 0; i < n_cases; i++) {
        char *dir = cases[i];
        char *case_ = path_join(prefix, dir);

        if (!case_) {
            continue;
        }

        ASSERT(!path_mkdir(case_, MODE_DEF, EXISTS_OK_DEF));
        free(case_);
    }

    ASSERT(path_mkdir(dir, MODE_DEF, 0));
    free(joined);
    RETURN_SCORE();
}

struct score path_test_main(void) {
    MODULE_INIT();
    FUNCTION_REPORT("path_norm()", test_norm());
    FUNCTION_REPORT("path_exists()", test_exists_true());
    FUNCTION_REPORT("!path_exists()", test_exists_false());
    FUNCTION_REPORT("path_is_abs()", test_is_abs_true());
    FUNCTION_REPORT("!path_is_abs()", test_is_abs_false());
    FUNCTION_REPORT("path_join()", test_join());
    FUNCTION_REPORT("path_url_fname()", test_url_fname());
    char *tmp_dir = path_mktempd();
    FUNCTION_REPORT("path_mkdir()", test_mkdir(tmp_dir));
    free(tmp_dir);
    MODULE_EXIT();
}

#include <stdio.h>
#include <stdlib.h>

#include "path.h"
#include "test.h"

struct path_case {
    char *path;
    char *expected;
};

SSSCORE test_norm(void) {
    SCORE_INIT();
    int n_cases = 23;
    struct path_case cases[] = {
        {"~root", "/root"},
        {"~root/dir/file", "/root/dir/file"},
        {"~unknownuser", "~unknownuser"},
        {"./", "."},
        {".", "."},
        {"", "."},
        {"./cmd", "cmd"},
        {"dir/", "dir"},
        {"./both/", "both"},
        {"dir/..", "."},
        {"dir1/dir2/..", "dir1"},
        {"dir1/dir2/../..", "."},
        {"dir1/dir2/../../..", ".."},
        {".hidden/..", "."},
        {"./..", ".."},
        {"..", ".."},
        {"../..", "../.."},
        {"../../..", "../../.."},
        {"/..", "/"},
        {"/../..", "/"},
        {"/../tmp", "/tmp"},
        {"/../../tmp", "/tmp"},
        {"../dir/..", ".."},
    };
    char *actual;

    for (int i = 0; i < n_cases; i++) {
        struct path_case case_ = cases[i];
        actual = path_norm(case_.path);

        if (!actual) {
            FAIL();
            continue;
        }

        int eq = path_eq(case_.expected, actual);
        ASSERT(eq);
        if (!eq) printf("\"%s\"\t\"%s\"\t\"%s\"\t\n", case_.path, case_.expected, actual);
        free(actual);
    }

    RETURN_SCORE();
}

SSSCORE test_basename(void) {
    SCORE_INIT();
    int n_cases = 5;
    struct path_case cases[] = {
        {"dir", "dir"},
        {"dir/file", "file"},
        {"dir1/dir2/dir3/file", "file"},
        {"/", "/"},
        {"~root", "root"},
    };

    for (int i = 0; i < n_cases; i++) {
        struct path_case case_ = cases[i];
        char *actual = path_basename(case_.path);
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
        "/dev",
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
        dne,
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
        "~root",
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
        "../dir/file",
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
        {".", "ftp://github.com/README.md", "README.md"},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_url_fname case_ = cases[i];
        char *actual = path_url_fname(case_.path, case_.url);

        if (!actual) {
            FAIL();
            continue;
        }

        ASSERT(path_eq(case_.expected, actual));
        free(actual);
    }

    RETURN_SCORE();
}

SSSCORE test_parent(void) {
    SCORE_INIT();
    int n_cases = 6;
    struct path_case cases[] = {
        {"/", "/"},
        {"/usr", "/"},
        {"usr", "."},
        {"/usr/", "/"},
        {"..", "."},
        {".", "."},
    };

    for (int i = 0; i < n_cases; i++) {
        struct path_case case_ = cases[i];
        char *actual = path_parent(case_.path);
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
    int n_cases = 11;
    struct case_join cases[] = {
        {".", "dir", "dir"},
        {"dir", "file", "dir/file"},
        {"/", "dev", "/dev"},
        {"tmp", "/home", "/home"},
        {"/tmp/dir", "", "/tmp/dir"},
        {"/tmp", "./..", "/"},
        {"/tmp", "./", "/tmp"},
        {"dir", "..", "."},
        {"dir1/dir2", "..", "dir1"},
        {"dir1/dir2", "../..", "."},
        {"dir1/dir2", "../../..", ".."},
    };

    for (int i = 0; i < n_cases; i++) {
        struct case_join case_ = cases[i];
        char *actual = path_join(case_.path, case_.other);

        if (!actual) {
            FAIL();
            continue;
        }

        int eq = path_eq(case_.expected, actual);
        ASSERT(eq);
        if (!eq) printf("\"%s\"\t\"%s\"\t\"%s\"\t\"%s\"\n", case_.path, case_.other, case_.expected, actual);
        free(actual);
    }

    RETURN_SCORE();
}

SSSCORE test_mkdir(char *prefix) {
    SCORE_INIT();
    printf("prefix = \"%s\";\n", prefix);
    int n_cases = 4;
    char *dir = "dir";
    char *joined = path_join(dir, "dir2");

    if (!joined) {
        RETURN_SCORE();
    }

    char *cases[] = {
        dir,
        joined,
        "dir3/dir4",
        dir,
    };

    for (int i = 0; i < n_cases; i++) {
        char *case_dir = cases[i];
        char *case_ = path_join(prefix, case_dir);

        if (!case_) {
            FAIL();
            printf("%s failed to join\n", case_dir);
            continue;
        }

        if (!path_mkdir(case_, MODE_DEF, EXISTS_OK_DEF)) {
            PASS();
            printf("%s was made\n", case_);
        } else {
            FAIL();
            printf("%s failed to make\n", case_);
        }

        free(case_);
    }

    if (path_mkdir(dir, MODE_DEF, 0)) {
        PASS();
        printf("%s expectingly failed to remake\n", dir);
    } else {
        FAIL();
        printf("%s remake did not fail: uh-oh\n", dir);
    }

    free(joined);
    RETURN_SCORE();
}

struct score path_test_main(void) {
    MODULE_INIT();
    FUNCTION_REPORT("path_norm()", test_norm());
    FUNCTION_REPORT("path_basename()", test_basename());
    FUNCTION_REPORT("path_exists()", test_exists_true());
    FUNCTION_REPORT("!path_exists()", test_exists_false());
    FUNCTION_REPORT("path_is_abs()", test_is_abs_true());
    FUNCTION_REPORT("!path_is_abs()", test_is_abs_false());
    FUNCTION_REPORT("path_parent()", test_parent());
    FUNCTION_REPORT("path_join()", test_join());
    FUNCTION_REPORT("path_url_fname()", test_url_fname());
    char *tmp_dir = path_mktempd();
    FUNCTION_REPORT("path_mkdir()", test_mkdir(tmp_dir));
    free(tmp_dir);
    MODULE_EXIT();
}

#include <stdio.h>
#include <stdlib.h>

#include "path.h"
#include "test.h"

struct path_case {
    char *path;
    char *expected;
};

TEST_CASE test_norm(void) {
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

TEST_CASE test_basename(void) {
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

TEST_CASE test_exists_true(void) {
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

TEST_CASE test_exists_false(void) {
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

TEST_CASE test_is_abs_true(void) {
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

TEST_CASE test_is_abs_false(void) {
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

TEST_CASE test_url_fname(void) {
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

TEST_CASE test_parent(void) {
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

static int rp_contains_path(rp_t **self, char *path) {
    rp_t *item;
    char *item_path;

    for (item = *self; item; item = item->next) {
        item_path = item->data;

        if (path_eq(path, item_path)) {
            return 1;
        }
    }

    return 0;
}

TEST_CASE test_is_dir(void) {
    SCORE_INIT();
    int n_cases = 7;
    char *tmp = path_mktempd();
    char *cwd = path_cwd();
    char *cases[] = {
        "/",
        "/tmp",
        "/home",
        "/etc",
        "/dev",
        tmp,
        cwd,
    };

    for (int i = 0; i < n_cases; i++) {
        char *case_ = cases[i];
        ASSERT(path_is_dir(case_));
    }

    free(cwd);
    free(tmp);
    RETURN_SCORE();
}

TEST_CASE test_list_dir_contains(rp_t **dir_list) {
    SCORE_INIT();
    int n_cases = 4;
    char *cases[] = {
        "/dev/null",
        "/dev/zero",
        "/dev/stdout",
        "/dev/stderr",
    };

    for (int i = 0; i < n_cases; i++) {
        ASSERT(rp_contains_path(dir_list, cases[i]));
    }

    RETURN_SCORE();
}

TEST_CASE test_list_dir_not_contains(rp_t **dir_list) {
    SCORE_INIT();
    int n_cases = 4;
    char *cases[] = {
        "/",
        "/dev",
        "/home",
        "/dev/disk/by-id"
    };

    for (int i = 0; i < n_cases; i++) {
        ASSERT(!rp_contains_path(dir_list, cases[i]));
    }

    RETURN_SCORE();
}

TEST_CASE test_list_dir(void) {
    SCORE_INIT();
    rp_t *dir_list = path_list_dir("/dev");
    SUBSCORE(test_list_dir_contains(&dir_list));
    SUBSCORE(test_list_dir_not_contains(&dir_list));
    rp_deep_free(&dir_list);
    RETURN_SCORE();
}

struct case_join {
    char *path;
    char *other;
    char *expected;
};

TEST_CASE test_join(void) {
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

TEST_CASE test_mkdir_none(char *prefix, char **cases, int n_cases) {
    SCORE_INIT();

    for (int i = 0; i < n_cases; i++) {
        char *case_dir = cases[i];
        char *case_ = path_join(prefix, case_dir);
        ASSERT(!path_mkdir(case_, MK_MODE_755, 0));
        free(case_);
    }

    RETURN_SCORE();
}

TEST_CASE test_mkdir_exists_ok(char *prefix, char **cases, int n_cases) {
    SCORE_INIT();

    for (int i = 0; i < n_cases; i++) {
        char *case_dir = cases[i];
        char *case_ = path_join(prefix, case_dir);
        ASSERT(!path_mkdir(case_, MK_MODE_755, MK_EXISTS_OK));
        free(case_);
    }

    RETURN_SCORE();
}

TEST_CASE test_mkdir_parent(char *prefix) {
    SCORE_INIT();
    int n_cases = 3;
    char *cases[] = {
        "dir1",
        "dir2/dir3",
        "dir4/dir5/dir6",
    };

    for (int i = 0; i < n_cases; i++) {
        char *case_dir = cases[i];
        char *case_ = path_join(prefix, case_dir);
        ASSERT(!path_mkdir(case_, MK_MODE_755, MK_PARENTS));
        free(case_);
    }

    RETURN_SCORE();
}

TEST_CASE test_mkdir(void) {
    SCORE_INIT();
    char *prefix1 = path_mktempd();
    char *prefix2 = path_mktempd();
    int n_cases = 3;
    char *cases[] = {
        "dir1",
        "dir2",
        "dir3",
    };

    SUBSCORE(test_mkdir_none(prefix1, cases, n_cases));
    SUBSCORE(test_mkdir_exists_ok(prefix1, cases, n_cases));
    SUBSCORE(test_mkdir_parent(prefix2));
    free(prefix1);
    free(prefix2);
    RETURN_SCORE();
};

TEST_MOD path_test_main(void) {
    SCORE_INIT();
    FUNCTION_REPORT("path_norm()", test_norm());
    FUNCTION_REPORT("path_basename()", test_basename());
    FUNCTION_REPORT("path_exists()", test_exists_true());
    FUNCTION_REPORT("!path_exists()", test_exists_false());
    FUNCTION_REPORT("path_is_abs()", test_is_abs_true());
    FUNCTION_REPORT("!path_is_abs()", test_is_abs_false());
    FUNCTION_REPORT("path_is_dir()", test_is_dir());
    FUNCTION_REPORT("path_list_dir()", test_list_dir());
    FUNCTION_REPORT("path_parent()", test_parent());
    FUNCTION_REPORT("path_join()", test_join());
    FUNCTION_REPORT("path_url_fname()", test_url_fname());
    FUNCTION_REPORT("path_mkdir()", test_mkdir());
    RETURN_SCORE();
}

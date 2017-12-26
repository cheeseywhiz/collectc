#include <stdio.h>
#include <stdlib.h>

#include "path.h"

void test_path_norm(void) {
    int n_cases = 6;
    char *cases[] = {
        "~",
        "~http",
        "~root/file",
        "./",
        ".",
        "./file",
        "./dir/"
    };

    for (int i = 0; i < n_cases; i++) {
        char *case_ = cases[i];
        char *norm = path_norm(case_);
        printf("\"%s\" -> \"%s\"\n", case_, norm);
        free(norm);
    }
}

int path_test_main(void) {
    printf("path_norm():\n");
    test_path_norm();

    char *home = path_home();
    printf("~/ == %s/\n", home);

    if (path_is_abs(home)) {
        printf("%s is an absolute path\n", home);
    } else {
        printf("%s is not an absolute path\n", home);
    }

    if (path_exists(home)) {
        printf("%s extsts\n", home);
    } else {
        printf("%s does not exist\n", home);
    }

    char *not_exists = "/does/not/exist";

    if (!path_exists(not_exists)) {
        printf("%s does not exist\n", not_exists);
    } else {
        printf("%s exists\n", not_exists);
    }

    char *tmp_dir = "/tmp/new_dir_that_will_exist_soon";

    if (!path_exists(tmp_dir)) {
        printf("%s does not exist\n", tmp_dir);
    } else {
        printf("%s exists\n", tmp_dir);
    }

    if (!path_mkdir(tmp_dir, MODE_DEF, EXISTS_OK_DEF)) {
        printf("Directory %s was made\n", tmp_dir);
    }

    if (path_exists(tmp_dir)) {
        printf("%s exists\n", tmp_dir);
        remove(tmp_dir);

        if (!path_exists(tmp_dir)) {
            printf("%s was removed\n", tmp_dir);
        }
    } else {
        printf("%s does not exist\n", tmp_dir);
    }


    char *root = "/";
    char *dev = path_join(root, "dev");

    if (path_exists(dev)) {
        printf("%s was properly joined from /\n", dev);
    } else {
        printf("%s does not exist\n", tmp_dir);
    }

    free(dev);

    char *desktop = path_join(home, "Desktop");

    if (path_exists(desktop)) {
        printf("%s was properly joined from ~\n", desktop);
    } else {
        printf("%s does not exist\n", desktop);
    }

    free(desktop);

    char *url = "http://i.imgur.com/i/image.jpg";
    char *path_url = path_url_fname(home, url);

    if (path_url != NULL) {
        printf("%s was properly formed from %s\n", path_url, url);
    } else {
        printf("%s was not properly formed from %s\n", path_url, url);
    }

    free(path_url);
    free(home);
    return 0;
}

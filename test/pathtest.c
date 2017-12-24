#include <stdio.h>
#include <stdlib.h>

#include "path.h"

int path_test_main(void) {
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
        printf("%s was properly joined\n", dev);
    } else {
        printf("%s does not exist\n", tmp_dir);
    }

    free(dev);

    char *desktop = path_join(home, "Desktop");

    if (path_exists(desktop)) {
        printf("%s was properly joined\n", desktop);
    } else {
        printf("%s does not exist\n", tmp_dir);
    }

    free(desktop);

    return 0;
}

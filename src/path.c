#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "path.h"
#include "reg.h"

char* path_home(void) {
    char *path;
    struct passwd *pwd = getpwuid(getuid());

    if (pwd) {
        path = pwd->pw_dir;
    } else {
        path = getenv("HOME");

        if (path == NULL) {
            path = "";
        }
    }

    return path;
}

/* TODO: varargs variant (self, other, other, other) */
char* path_join(char *path, char *other) {
    if (path_is_abs(other)) {
        return other;
    }

    size_t other_len = strlen(other);

    if (!other_len) {
        return path;
    }

    if (path_cmp(path, "/")) {
        path = "";
    }

    ssize_t new_path_len = strlen(path) + 1 + other_len;
    char *new_path_buf = calloc(new_path_len + 1, 1);

    if (sprintf(new_path_buf, "%s/%s", path, other) != new_path_len) {
        free(new_path_buf);
        return NULL;
    } else {
        return new_path_buf;
    }
}

int path_cmp(char *path, char *other) {
    return strcmp(path, other) == 0;
}

int path_exists(char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

int path_is_abs(char *path) {
    return regex_starts_with(path, "/");
}

int path_open_write(char *path) {
    int flags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    return open(path, flags, mode);
}

int path_mkdir(char *path, int mode, int exists_ok) {
    if (exists_ok && path_exists(path)) {
        return 0;
    } else {
        return mkdir(path, mode);
    }
}

char* path_url_fname(char *path, char *url) {
    char *new_fname = regex_url_fname(url);

    if (!new_fname) {
        return NULL;
    }

    char *new_path = path_join(path, new_fname);
    free(new_fname);
    return new_path;
}

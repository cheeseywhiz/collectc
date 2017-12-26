#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>
#include <regex.h>

#include "path.h"
#include "reg.h"

static char* home_dir_user(char *username) {
    struct passwd *pwd = getpwnam(username);

    if (!pwd) {
        return NULL;
    }

    return strdup(pwd->pw_dir);
}

char* path_expand_user(char *path) {
    if (path_eq(path, "~")) {
        return path_home();
    }

    path = strdup(path);

    if (!path) {
        return NULL;
    }

    char *pattern = "^~([^/]*)";
    char *user = regex_match_one_subexpr(pattern, path, REG_EXTENDED);

    if (!user) {
        return path;
    }

    char *home;

    if (strlen(user)) {
        home = home_dir_user(user);
    } else {
        home = path_home();
    }

    if (!home) {
        goto cleanup;
    }

    char *after_home = regex_str_slice(path, 1 + strlen(user), strlen(path));

    if (!after_home) {
        free(home);
        goto fail;
    } else if (!strlen(after_home)) {
        free(after_home);
        free(path);
        path = home;
        goto cleanup;
    }

    char *new_path = path_join(home, after_home + 1);
    free(after_home);
    free(home);

    if (!new_path) {
        goto fail;
    }

    free(path);
    path = new_path;
    goto cleanup;

fail:
    free(path);
    path = NULL;

cleanup:
    free(user);
    return path;
}

static char* trim_trailing_slash(char *path) {
    path = strdup(path);

    if (!path) {
        return NULL;
    }

    if (!path_eq(path, "/") && regex_ends_with(path, "/")) {
        char *new_path = regex_str_slice(path, 0, strlen(path) - 1);
        free(path);

        if (!new_path) {
            return NULL;
        }

        path = new_path;
    }

    return path;
}

static char* trim_initial_point_slash(char *path) {
    path = strdup(path);

    if (!path) {
        return NULL;
    }

    if (regex_starts_with(path, "./")) {
        char *new_path = regex_str_slice(path, 2, strlen(path));
        free(path);

        if (!new_path) {
            return NULL;
        }

        path = new_path;
    }

    return path;
}

char* path_norm(char *path) {
    char *new_path;
    path = strdup(path);

    if (!path) {
        return NULL;
    }

    new_path = path_expand_user(path);
    free(path);

    if (!new_path) {
        return NULL;
    }

    path = new_path;
    new_path = trim_trailing_slash(path);
    free(path);

    if (!new_path) {
        return NULL;
    }

    path = new_path;
    new_path = trim_initial_point_slash(path);
    free(path);

    if (!new_path) {
        return NULL;
    }

    return new_path;
}

char* path_home(void) {
    char *path;
    struct passwd *pwd = getpwuid(getuid());

    if (pwd) {
        path = pwd->pw_dir;
    } else {
        path = getenv("HOME");

        if (path == NULL) {
            return NULL;
        }
    }

    return strdup(path);
}

/* TODO: varargs variant (self, other, other, other) */
char* path_join(char *path, char *other) {
    if (path_is_abs(other)) {
        return strdup(other);
    }

    size_t other_len = strlen(other);

    if (!other_len) {
        return strdup(path);
    }

    if (path_eq(path, "/")) {
        path = "";
    }

    size_t new_path_len = strlen(path) + 1 + other_len;
    char *new_path_buf = calloc(new_path_len + 1, 1);

    if (sprintf(new_path_buf, "%s/%s", path, other) < 0) {
        free(new_path_buf);
        return NULL;
    } else {
        char *norm_path = path_norm(new_path_buf);
        free(new_path_buf);

        if (!norm_path) {
            return NULL;
        }

        return norm_path;
    }
}

int path_eq(char *path, char *other) {
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

    char *new_path;
    new_path = path_join(path, new_fname);
    free(new_fname);

    if (!new_path) {
        return NULL;
    }

    path = new_path;
    new_path = path_norm(path);
    free(path);

    if (!new_path) {
        return NULL;
    }

    return new_path;
}

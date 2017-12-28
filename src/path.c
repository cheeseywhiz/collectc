#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>
#include <regex.h>
#include <limits.h>

#include "path.h"
#include "reg.h"

static char* home_dir_user(char *username) {
    struct passwd *pwd = getpwnam(username);

    if (!pwd) {
        return NULL;
    }

    return strdup(pwd->pw_dir);
}

static char* expand_user(char *path) {
    path = strdup(path);

    if (!path) {
        return NULL;
    }

    char *pattern = "^~([^/]*)";
    char *user = regex_match_one_subexpr(pattern, path, REG_EXTENDED);
    char *home;

    if (!user) {
        return path;
    } else if (strlen(user)) {
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
    free(path);
    path = new_path;
    free(after_home);
    free(home);

    if (!path) {
        goto fail;
    }

    goto cleanup;

fail:
    free(path);
    path = NULL;

cleanup:
    free(user);
    return path;
}

static char* trim_path_ends(char *path) {
    path = strdup(path);

    if (!path) {
        return NULL;
    }

    if (!path_eq(path, "/") && regex_ends_with(path, "/")) {
        char *new_path = regex_str_slice(path, 0, strlen(path) - 1);
        free(path);
        path = new_path;

        if (!path) {
            return NULL;
        }
    }

    if (regex_starts_with(path, "./")) {
        char *new_path = regex_str_slice(path, 2, strlen(path));
        free(path);
        path = new_path;

        if (!path) {
            return NULL;
        }
    }

    return path;
}

char* path_norm(char *path) {
    path = strdup(path);

    if (!path) {
        return NULL;
    }

    char *new_path = expand_user(path);
    free(path);
    path = new_path;

    if (!path) {
        return NULL;
    }

    new_path = trim_path_ends(path);
    free(path);
    path = new_path;

    if (!path) {
        return NULL;
    }

    return path;
}

char* path_home(void) {
    char *path;
    struct passwd *pwd = getpwuid(getuid());

    if (pwd) {
        path = pwd->pw_dir;
    } else {
        path = getenv("HOME");

        if (!path) {
            return NULL;
        }
    }

    return strdup(path);
}

char* path_cwd(void) {
#ifdef _GNU_SOURCE
    return get_current_dir_name();
#else
    size_t size = 1024;
    char buf[size + 1];
    getcwd(buf, size);
    return strdup(buf);
#endif /* _GNU_SOURCE */
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
    char *norm = path_norm(path);

    if (!norm) {
        return 0;
    }

    int exists = stat(norm, &st) == 0;
    free(norm);
    return exists;
}

int path_is_abs(char *path) {
    char *norm = path_norm(path);

    if (!norm) {
        return 0;
    }

    int is_abs = regex_starts_with(norm, "/");
    free(norm);
    return is_abs;
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

char* path_mktempd(void) {
    char template[PATH_MAX] = "/tmp/collectc.XXXXXX";
    char *tmp = strdup(mkdtemp(template));
    return tmp;
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

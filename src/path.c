#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>
#include <regex.h>
#include <limits.h>
#include <dirent.h>

#include "path.h"
#include "reg.h"
#include "random_popper.h"

static char* home_dir_user(char *username) {
    struct passwd *pwd = getpwnam(username);

    if (!pwd) {
        return NULL;
    }

    return strdup(pwd->pw_dir);
}

static char* basename_as_is(char *path) {
    return strdup(basename(path));
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

static char* fix_double_dots(char *path) {
    path = strdup(path);

    if (!path) {
        return NULL;
    } else if (!regex_contains(path, "..")) {
        return path;
    }

    char *new_path;

    if (regex_starts_with(path, "/../")) {
        new_path = regex_str_slice(path, 3, strlen(path));
        free(path);
        path = new_path;

        if (!path) {
            return NULL;
        }

        new_path = fix_double_dots(path);
        free(path);
        path = new_path;
        return path;
    } else if (path_eq(path, "/..")) {
        free(path);
        return strdup("/");
    }

    char *pattern = "([^/]*/\\.\\./?)";
    char *needle = regex_match_one_subexpr(pattern, path, 0);

    if (!needle) {
        return path;
    } else if (regex_starts_with(needle, "../..") || regex_starts_with(needle, "./..")) {
        free(needle);
        return path;
    }

    new_path = regex_remove(path, needle);
    free(needle);
    free(path);
    path = new_path;
    new_path = fix_double_dots(path);
    free(path);
    path = new_path;
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

    new_path = fix_double_dots(path);
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

    if (!strlen(path)) {
        free(path);
        path = strdup(".");
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

char* path_basename(char *path) {
    path = path_norm(path);

    if (!path) {
        return NULL;
    }

    char *new_path;
    new_path = basename_as_is(path);
    free(path);
    path = new_path;
    return path;
}

char* path_parent(char *path) {
    path = path_norm(path);

    if (!path) {
        return NULL;
    }

    char *new_path = strdup(dirname(path));
    free(path);
    path = new_path;
    return path;
}

/* TODO: varargs variant (self, other, other, other) */
char* path_join(char *path, char *other) {
    path = path_norm(path);

    if (!path) {
        return NULL;
    }

    other = path_norm(other);

    if (!other) {
        free(path);
        return NULL;
    }

    if (path_is_abs(other)) {
        free(path);
        return other;
    }

    size_t other_len = strlen(other);

    if (!other_len) {
        free(other);
        return path;
    } else if (path_eq(other, ".")) {
        free(other);
        other = strdup("");
    }

    if (path_eq(path, "/")) {
        free(path);
        path = strdup("");
    }

    size_t new_path_len = strlen(path) + 1 + other_len;
    char new_path_buf[new_path_len + 1];
    int spf_err = sprintf(new_path_buf, "%s/%s", path, other) < 0;
    free(other);
    free(path);

    if (spf_err) {
        return NULL;
    }

    return path_norm(new_path_buf);
}

char* path_abspath(char *path) {
    char *cwd = path_cwd();
    char *abs = path_join(cwd, path);
    free(cwd);
    return abs;
}

int path_eq(char *path, char *other) {
    return strcmp(path, other) == 0;
}

int path_exists(char *path) {
    path = path_norm(path);

    if (!path) {
        return 0;
    }

    struct stat st;
    int exists = stat(path, &st) == 0;
    free(path);
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

int path_is_dir(char *path) {
    path = path_norm(path);

    if (!path) {
        return 1;
    }

    struct stat st;

    if (stat(path, &st)) {
        free(path);
        return -1;
    }

    /* sys/stat.h */
    int is_dir = S_ISDIR(st.st_mode);
    free(path);
    return is_dir;
}

static rp_t* append_entry_path(rp_t **self, char *path, struct dirent *entry) {
    char *entry_path = path_join(path, entry->d_name);

    if (!entry_path) {
        return NULL;
    }

    rp_t *new_item = rp_append(self, entry_path);

    if (!new_item) {
        free(entry_path);
        return NULL;
    }

    return new_item;
}

rp_t* path_list_dir(char *path) {
    path = path_norm(path);

    if (!path) {
        return NULL;
    } else if (!path_is_dir(path)) {
        free(path);
        return NULL;
    }

    DIR *dir = opendir(path);

    if (!dir) {
        free(path);
        return NULL;
    }

    rp_t *dir_list = NULL;
    struct dirent *entry;

    for (entry = readdir(dir); entry; entry = readdir(dir)) {
        if (path_eq(entry->d_name, ".") || path_eq(entry->d_name, "..")) {
            continue;
        } else if (!append_entry_path(&dir_list, path, entry)) {
            break;
        }
    }

    free(path);

    if (closedir(dir)) {
        rp_deep_free(&dir_list);
        return NULL;
    };

    return dir_list;
}

char* path_random(char *path) {
    rp_t *dir_list = path_list_dir(path);

    if (!dir_list) {
        return NULL;
    }

    char *random = rp_pop_random(&dir_list);
    rp_deep_free(&dir_list);
    return random;
}

int path_open_write(char *path) {
    path = path_norm(path);

    if (!path) {
        return -1;
    }

    int flags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    int fd = open(path, flags, mode);
    free(path);
    return fd;
}

int path_mkdir(char *path, int mode, int mk_flags) {
    path = path_abspath(path);

    if (!path) {
        return 1;
    } else if ((mk_flags & MK_EXISTS_OK) && path_exists(path)) {
        free(path);
        return 0;
    } 

    int path_failed, parent_failed;
    path_failed = mkdir(path, mode);

    if (!path_failed) {
        free(path);
        return 0;
    } else if (!(mk_flags & MK_PARENTS)) {
        free(path);
        return 1;
    }

    char *parent = path_parent(path);
    parent_failed = path_mkdir(parent, mode, MK_EXISTS_OK | mk_flags);
    free(parent);

    if (parent_failed) {
        free(path);
        return parent_failed;
    }

    path_failed = path_mkdir(path, mode, mk_flags);
    free(path);
    return path_failed;
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

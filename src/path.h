#ifndef PATH_H
#define PATH_H

#include <sys/stat.h>
#include <sys/types.h>

#include "random_popper.h"

char* path_norm(char *path);
char* path_home(void);
char* path_cwd(void);
char* path_basename(char *path);
char* path_parent(char *path);
char* path_join(char *path, char *other);
char* path_abspath(char *path);
int path_eq(char *path, char *other);
int path_exists(char *path);
int path_is_abs(char *path);
int path_is_dir(char *path);
rp_t* path_list_dir(char *path);
char* path_random(char *path);
int path_open_write(char *path);

#define MK_MODE_755 S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

enum path_mkdir_flags {
    MK_PARENTS = 1 << 0,
    MK_EXISTS_OK = 1 << 1,
};

int path_mkdir(char *path, int mode, int mk_flags);
char* path_mktempd(void);
char* path_url_fname(char *path, char *url);

#endif

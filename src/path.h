#ifndef PATH_H
#define PATH_H

#include <sys/stat.h>
#include <sys/types.h>

char* path_norm(char *path);
char* path_home(void);
char* path_cwd(void);
char* path_join(char *path, char *other);
int path_eq(char *path, char *other);
int path_exists(char *path);
int path_is_abs(char *path);
int path_open_write(char *path);

#define MODE_DEF S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
#define EXISTS_OK_DEF 1

int path_mkdir(char *path, int mode, int exists_ok);
char* path_mktempd(void);
char* path_url_fname(char *path, char *url);

#endif

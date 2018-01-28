#include <stdlib.h>

#include "auth.h"
#include "path.h"

static void init_config_folder_paths(char *folders[]) {
    folders[0] = path_cwd();

    folders[1] = NULL;
    char *xdg_config_home_env = getenv("XDG_CONFIG_HOME");

    if (xdg_config_home_env) {
        folders[1] = path_norm(xdg_config_home_env);
    }

    folders[2] = NULL;
    char *home_env = getenv("HOME");

    if (home_env) {
        folders[2] = path_join(home_env, ".config");
    }
}

static char* config_file_in_dir(char *config_dir) {
    if (!config_dir) return NULL;

    char *config_file = path_join(config_dir, "praw.ini");

    if (config_file && !path_exists(config_file)) {
        free(config_file);
        config_file = NULL;
    }

    return config_file;
}

char* auth_config_path(void) {
    char *folders[AUTH_N_CONFIG_FOLDERS];
    init_config_folder_paths(folders);
    char *config_path = NULL;
    int i;

    for (i = 0; i < AUTH_N_CONFIG_FOLDERS; i++) {
        config_path = config_file_in_dir(folders[i]);
        if (config_path) break;
    }

    for (; i < AUTH_N_CONFIG_FOLDERS; i++) {
        char *path = folders[i];
        if (path) free(path);
    }

    return config_path;
}

#include <stdlib.h>

#include "auth.h"
#include "path.h"
#include "jsmnutils.h"
#include "ini.h"
#include "reg.h"

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

    for (i = 0; i < AUTH_N_CONFIG_FOLDERS && !config_path; i++) {
        config_path = config_file_in_dir(folders[i]);
    }

    for (i = 0; i < AUTH_N_CONFIG_FOLDERS; i++) {
        char *path = folders[i];
        if (path) free(path);
    }

    return config_path;
}

ju_json_t* auth_parse_config(void) {
    char *config_path = auth_config_path();
    if (!config_path) return NULL;

    ju_json_t *json = ini_parse_path(config_path);
    free(config_path);
    return json;
}

static char* config_get_key(ju_json_t *config, int profile_index, char *key) {
    int key_index = ju_object_get(config, profile_index, key);
    if (key_index < 0) return NULL;

    return regex_str_slice(config->json_str, config->tokens[key_index].start, config->tokens[key_index].end);
}

#define __GET_KEY(key) \
    key_copy = config_get_key(config, profile_index, key); \
    if (!key_copy) { \
        ret = 2; \
        goto exit; \
    }

int auth_init_profile(struct auth_profile *profile, char *name) {
    ju_json_t *config = auth_parse_config();
    if (!config) return 1;

    int profile_index = ju_object_get(config, 0, name);

    if (profile_index < 0) {
        ini_free_path(config);
        return profile_index;
    }

    char *key_copy;
    int ret = 0;

    profile->username = __GET_KEY("username");
    profile->password = __GET_KEY("password");
    profile->client_id = __GET_KEY("client_id");
    profile->secret = __GET_KEY("client_secret");

exit:
    ini_free_path(config);
    return ret;
}

void auth_free_profile(struct auth_profile *self) {
    free(self->secret);
    free(self->client_id);
    free(self->password);
    free(self->username);
}

int auth_init_default_profile(struct auth_profile *profile) {
    return auth_init_profile(profile, "DEFAULT");
}

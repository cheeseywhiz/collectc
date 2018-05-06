#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "log.h"
#include "auth.h"
#include "path.h"
#include "jsmnutils.h"
#include "ini.h"
#include "reg.h"
#include "get.h"

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

    ju_json_t *json = ini_parse_file(config_path);
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
        return ret; \
    }

int auth_init_profile(struct auth_profile *profile, ju_json_t *config, char *name) {
    int profile_index = ju_object_get(config, 0, name);

    if (profile_index < 0) {
        return profile_index;
    }

    char *key_copy;
    int ret = 0;

    profile->username = __GET_KEY("username");
    profile->password = __GET_KEY("password");
    profile->client_id = __GET_KEY("client_id");
    profile->secret = __GET_KEY("client_secret");

    return ret;
}

void auth_free_profile(struct auth_profile *self) {
    free(self->secret);
    free(self->client_id);
    free(self->password);
    free(self->username);
}

int auth_init_default_profile(struct auth_profile *profile) {
    ju_json_t *config = auth_parse_config();
    if (!config) return -1;

    int ret = auth_init_profile(profile, config, "DEFAULT");
    ini_free_parsed(config);
    return ret;
}

static int init_auth_tok_handle(struct get_handle *handle, ju_json_t *config, char *name) {
    int ret = get_init_handle(handle);
    if (ret) return ret;

    struct auth_profile profile;
    ret = auth_init_profile(&profile, config, name);

    if (ret) {
        get_cleanup_handle(handle);
        return ret;
    }

    char *user_pwd_fmt = "%s:%s";
    size_t user_pwd_len = strlen(user_pwd_fmt) - 4 + strlen(profile.client_id) + strlen(profile.secret);
    handle->user_pwd = malloc(user_pwd_len + 1);

    if (!handle->user_pwd) {
        LOG_ERRNO();
        return -1;
    }

    sprintf(handle->user_pwd, user_pwd_fmt, profile.client_id, profile.secret);

    char *post_data_fmt = "grant_type=password&username=%s&password=%s";
    size_t post_data_len = strlen(post_data_fmt) - 4 + strlen(profile.username) + strlen(profile.password);
    handle->post_data = malloc(post_data_len + 1);

    if (!handle->post_data) {
        LOG_ERRNO();
        return -1;
    }

    sprintf(handle->post_data, post_data_fmt, profile.username, profile.password);

    curl_easy_setopt(handle->handle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(handle->handle, CURLOPT_USERPWD, handle->user_pwd);
    curl_easy_setopt(handle->handle, CURLOPT_POSTFIELDS, handle->post_data);

    auth_free_profile(&profile);
    return 0;
}

static void cleanup_auth_tok_handle(struct get_handle *handle) {
    free(handle->user_pwd);
    free(handle->post_data);
    get_cleanup_handle(handle);
}

char* auth_get_access_token(ju_json_t *config, char *name) {
    struct get_handle handle;
    if (init_auth_tok_handle(&handle, config, name)) return NULL;

    ju_json_t *json = get_json(&handle, "https://www.reddit.com/api/v1/access_token");
    cleanup_auth_tok_handle(&handle);
    if (!json) return NULL;

    int token_index = ju_object_get(json, 0, "access_token");

    if (token_index < 0) {
        get_free_json(json);
        return NULL;
    }

    char *access_token = regex_str_slice(json->json_str, json->tokens[token_index].start, json->tokens[token_index].end);
    get_free_json(json);
    return access_token;
}

char* auth_get_default_access_token(void) {
    ju_json_t *config = auth_parse_config();
    if (!config) return NULL;

    char *token = auth_get_access_token(config, "DEFAULT");
    ini_free_parsed(config);
    return token;
}

int auth_init_handle(struct get_handle *handle, ju_json_t *config, char *name) {
    int ret = get_init_handle(handle);
    if (ret) return ret;

    char *token = auth_get_access_token(config, name);

    if (!token) {
        get_cleanup_handle(handle);
        return -1;
    }

    char *auth_bearer_fmt = "Authorization: bearer %s";
    size_t auth_bearer_len = strlen(auth_bearer_fmt) - 2 + strlen(token);
    char auth_bearer[auth_bearer_len + 1];
    sprintf(auth_bearer, auth_bearer_fmt, token);
    free(token);

    handle->headers = NULL;
    handle->headers = curl_slist_append(handle->headers, auth_bearer);

    if (!handle->headers) {
        get_cleanup_handle(handle);
        return -1;
    }

    curl_easy_setopt(handle->handle, CURLOPT_HTTPHEADER, handle->headers);
    return 0;
}

void auth_cleanup_handle(struct get_handle *handle) {
    curl_slist_free_all(handle->headers);
    get_cleanup_handle(handle);
}

int auth_init_default_handle(struct get_handle *handle) {
    ju_json_t *config = auth_parse_config();
    if (!config) return -1;

    int ret = auth_init_handle(handle, config, "DEFAULT");
    ini_free_parsed(config);
    return ret;
}

ju_json_t* auth_get_reddit(struct get_handle *handle, char *url) {
    char *url_fmt = "https://oauth.reddit.com/%s";
    size_t url_len = strlen(url_fmt) - 2 + strlen(url);
    char auth_url[url_len + 1];
    sprintf(auth_url, url_fmt, url);

    ju_json_t *json = get_json(handle, auth_url);
    return json;
}

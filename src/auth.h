#ifndef AUTH_H
#define AUTH_H

#include "jsmnutils.h"
#include "get.h"

struct auth_profile {
    char *username;
    char *password;
    char *client_id;
    char *secret;
};

#define AUTH_N_CONFIG_FOLDERS 3

char* auth_config_path(void);
ju_json_t* auth_parse_config(void);
int auth_init_profile(struct auth_profile *profile, ju_json_t *config, char *name);
int auth_init_default_profile(struct auth_profile *profile);
void auth_free_profile(struct auth_profile *profile);
char* auth_get_access_token(ju_json_t *config, char *name);
char* auth_get_default_access_token(void);
int auth_init_handle(struct get_handle *handle, ju_json_t *config, char *name);
void auth_cleanup_handle(struct get_handle *handle);
int auth_init_default_handle(struct get_handle *handle);
ju_json_t* auth_get_reddit(struct get_handle *handle, char *url);

#endif

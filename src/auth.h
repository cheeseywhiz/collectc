#ifndef AUTH_H
#define AUTH_H

#include "jsmnutils.h"

#define AUTH_N_CONFIG_FOLDERS 3

char* auth_config_path(void);
ju_json_t* auth_parse_config(void);

#endif

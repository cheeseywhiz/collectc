#ifndef INI_H
#define INI_H

#include "jsmnutils.h"

ju_json_t* ini_parse(char *ini_str);
ju_json_t* ini_parse_path(char *path);
void ini_free_path(ju_json_t *json);

#endif

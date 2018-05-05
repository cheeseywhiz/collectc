#ifndef INI_H
#define INI_H

#include "jsmnutils.h"

ju_json_t* ini_parse(char *ini_str);
ju_json_t* ini_parse_file(char *path);
void ini_free_parsed(ju_json_t *json);

#endif

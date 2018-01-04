#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "path.h"

int log_level = LOG_CRITICAL | LOG_ERROR | LOG_WARNING;
char prog_name[LOG_NAME_SIZE] = "";

void log_init_prog_name(char *argv[]) {
    char *path = path_norm(argv[0]);

    if (!path) {
        free(path);
        return;
    }

    char *basename = path_basename(path);
    free(path);

    if (!basename) {
        return;
    }

    sprintf(prog_name, "%s: ", basename);
    free(basename);
}

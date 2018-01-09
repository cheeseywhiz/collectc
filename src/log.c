#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

#include "log.h"
#include "path.h"

int log_level = LOG_CRITICAL | LOG_ERROR | LOG_WARNING;
int log_strict_exceptions = 1;
int log_surpress_exceptions = 0;
char prog_name[LOG_NAME_SIZE] = "";

void log_init_prog_name(char *argv[]) {
    char *path = path_norm(argv[0]);

    if (!path) {
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

void log_backtrace(void) {
    /* Implementation from https://www.gnu.org/software/libc/manual/html_node/Backtraces.html */
    size_t actual_size, array_size = 1 << 4;
    void *array[array_size];
    char **strings;

    actual_size = backtrace(array, array_size);
    strings = backtrace_symbols(array, actual_size);

    STDERR("%s", prog_name);
    STDERR("backtrace: \n");

    for (size_t i = 1; i < actual_size; i++) {
        STDERR("%s\n", strings[i]);
    }

    free(strings);
}

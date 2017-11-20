#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg.h"

char* regex_match_one_subexpr(char *pattern, char *haystack, int cflags) {
    char *needle;
    regex_t reg;
    int n_matches = 2;
    regmatch_t pmatch[n_matches];

    if (regcomp(&reg, pattern, cflags) != 0) goto fail;
    if (regexec(&reg, haystack, n_matches, pmatch, 0) != 0) goto fail;

    needle = regex_str_slice(haystack, pmatch[1].rm_so, pmatch[1].rm_eo);

    if (needle) {
        goto cleanup;
    };

fail:
    needle = malloc(1);
    strcpy(needle, "");

cleanup:
    regfree(&reg);
    return needle;
};

int regex_starts_with(char *haystack, char *needle) {
    size_t ned_len = strlen(needle);
    size_t hay_len = strlen(haystack);

    for (int i = 0; ; i++) {
        if (i == ned_len) {
            break;
        } else if ((i == hay_len) || (i > ned_len) || (i > hay_len) || (*(haystack + i) != *(needle + i))) {
            return 0;
        };
    };

    return 1;
};

char* regex_str_slice(char *src, int start, int end) {
    int dest_length = end - start;
    int src_length = strlen(src);

    if (dest_length > 0 && 0 <= start && start < src_length && 0 < end && end <= src_length) {
        char *dest = malloc(dest_length + 1);

        if (!dest) {
            return NULL;
        };

        bzero(dest, dest_length + 1);
        return strncpy(dest, src + start, dest_length);
    } else if (dest_length == 0) {
        char *dest = malloc(1);
        *dest = 0;
        return dest;
    };

    return NULL;
};

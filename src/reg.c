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

    int start = pmatch[1].rm_so;
    int end = pmatch[1].rm_eo;
    int needle_length = end - start;
    int haystack_length = strlen(haystack);

    if (needle_length > 0 && 0 <= start && start < haystack_length && 0 < end && end <= haystack_length) {
        needle = malloc(needle_length + 1);
        bzero(needle, needle_length + 1);
        needle = strncpy(needle, haystack + start, needle_length);
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
    for (int i = 0;; i++) {
        if (i == strlen(needle)) {
            break;
        } else if ((i == strlen(haystack)) || (i > strlen(needle)) || (i > strlen(haystack)) || (*(haystack + i) != *(needle + i))) {
            return 0;
        };
    };

    return 1;
};

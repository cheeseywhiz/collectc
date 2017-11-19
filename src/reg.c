#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg.h"
#include "stringutil.h"

char* regex_match_one_subexpr(char *pattern, char *haystack, int cflags) {
    char *needle;
    regex_t reg;
    int n_matches = 2;
    regmatch_t pmatch[n_matches];

    if (regcomp(&reg, pattern, cflags) != 0) goto fail;
    if (regexec(&reg, haystack, n_matches, pmatch, 0) != 0) goto fail;

    needle = str_slice(haystack, pmatch[1].rm_so, pmatch[1].rm_eo);

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
    for (int i = 0; ; i++) {
        if (i == strlen(needle)) {
            return 1;
        } else if ((i == strlen(haystack)) || (i > strlen(needle)) || (i > strlen(haystack)) || (*(haystack + i) != *(needle + i))) {
            return 0;
        };
    };
};

#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "reg.h"

#ifndef _GNU_SOURCE
char* strdup(char *str) {
    char *new = calloc(strlen(str) + 1, 1);

    if (!new) {
        return NULL;
    }

    strcpy(new, str);
    return new;
}
#endif /* _GNU_SOURCE */

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
    }

fail:
    needle = NULL;

cleanup:
    regfree(&reg);
    return needle;
}

int regex_contains(char *needle, char *haystack) {
    size_t needle_len = strlen(needle);
    ssize_t pattern_len = 1 + needle_len + 1;
    char *pattern = calloc(pattern_len + 1, 1);

    if (sprintf(pattern, "(%s)", needle) != pattern_len) {
        return 0;
    };

    char *match = regex_match_one_subexpr(pattern, haystack, REG_EXTENDED);

    if (!match) {
        free(pattern);
        return 0;
    }

    int contains = strlen(match) != 0;
    free(pattern);
    free(match);
    return contains;
}

static char* str_rev(char *s) {
    size_t s_len = strlen(s);
    char *new = calloc(s_len + 1, 1);

    if (!new) {
        return NULL;
    }

    for (size_t i = 0; i < s_len; i++) {
        new[i] = s[s_len - 1 - i];
    }

    return new;
}

int regex_starts_with(char *haystack, char *needle) {
    size_t ned_len = strlen(needle);
    size_t hay_len = strlen(haystack);

    for (size_t i = 0; i < ned_len; i++) {
        if ((i >= ned_len) || (i >= hay_len) || (haystack[i] != needle[i])) {
            return 0;
        }
    }

    return 1;
}

int regex_ends_with(char *haystack, char *needle) {
    char *hay_rev = str_rev(haystack);

    if (!hay_rev) {
        return 0;
    }

    char *needle_rev = str_rev(needle);

    if (!needle_rev) {
        free(hay_rev);
        return 0;
    }

    int ends_with = regex_starts_with(hay_rev, needle_rev);
    free(needle_rev);
    free(hay_rev);
    return ends_with;
}

char* regex_str_slice(char *src, int start, int end) {
    int dest_length = end - start;
    int src_length = strlen(src);

    if (dest_length > 0 && 0 <= start && start < src_length && 0 < end && end <= src_length) {
        char *dest = calloc(dest_length + 1, 1);

        if (!dest) {
            return NULL;
        }

        return strncpy(dest, src + start, dest_length);
    } else if (dest_length == 0) {
        return calloc(1, 1);
    } else {
        return NULL;
    };
}

char* regex_url_fname(char *url) {
    int n_matches = 6;
    regex_t reg;
    regmatch_t pmatch[n_matches];
    char *pattern = "^((http[s]?|ftp)://)?([^/]*)(/([^/?#]*))*";
    regcomp(&reg, pattern, REG_EXTENDED);
    char *fname;

    if (regexec(&reg, url, n_matches, pmatch, 0) != 0) {
        fname = calloc(1, 1);
    } else {
        fname = regex_str_slice(url, pmatch[5].rm_so, pmatch[5].rm_eo);
    }

    regfree(&reg);
    return fname;
}

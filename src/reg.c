#include <regex.h>
#include <stdlib.h>
#include <string.h>

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

    if (regcomp(&reg, pattern, REG_EXTENDED | cflags)) goto fail;
    if (regexec(&reg, haystack, n_matches, pmatch, 0)) goto fail;

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

int regex_contains(char *haystack, char *needle) {
    return regex_find(haystack, needle) != strlen(haystack);
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

size_t regex_find(char *haystack, char *needle) {
    size_t i;

    for (i = 0; i < strlen(haystack); i++) {
        if (regex_starts_with(haystack + i, needle)) {
            break;
        }
    }

    return i;
}

static char* remove_i(char *haystack, char *needle, size_t i) {
    size_t hay_len = strlen(haystack);
    size_t ned_len = strlen(needle);
    char *before = regex_str_slice(haystack, 0, i);

    if (!before) {
        return NULL;
    }

    char *after = regex_str_slice(haystack, i + ned_len, hay_len);

    if (!after) {
        free(before);
        return NULL;
    }

    size_t before_len = strlen(before);
    size_t after_len = strlen(after);
    char *ptr = realloc(before, before_len + after_len + 1);

    if (!ptr) {
        free(after);
        free(before);
        return NULL;
    }    

    before = strcat(ptr, after);
    free(after);
    return before;
}

char* regex_remove(char *haystack, char *needle) {
    size_t ned_i = regex_find(haystack, needle);

    if (ned_i == strlen(haystack)) {
        return NULL;
    }

    return remove_i(haystack, needle, ned_i);
}

char* regex_remove_first_pattern(char *haystack, char *pattern, int cflags) {
    char *needle = regex_match_one_subexpr(pattern, haystack, cflags);

    if (!needle) {
        return strdup(haystack);
    }

    char *trimmed = regex_remove(haystack, needle);
    free(needle);
    return trimmed;
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

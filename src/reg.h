#ifndef REG_H
#define REG_H

char* regex_match_one_subexpr(char *pattern, char *haystack, int cflags);
int regex_contains(char *haystack, char *needle);
int regex_starts_with(char *haystack, char *needle);
int regex_ends_with(char *haystack, char *needle);
size_t regex_find(char *haystack, char *needle);
char* regex_remove(char *haystack, char *needle);
char* regex_remove_first_pattern(char *haystack, char *pattern, int cflags);
char* regex_str_slice(char *src, int start, int end);
char* regex_url_fname(char *url);

#endif

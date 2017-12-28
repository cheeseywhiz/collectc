#ifndef REG_H
#define REG_H

#ifndef _GNU_SOURCE
char* strdup(char *str);
#endif /* _GNU_SOURCE */

char* regex_match_one_subexpr(char*, char*, int);
int regex_contains(char *needle, char *haystack);
int regex_starts_with(char*, char*);
int regex_ends_with(char *haystack, char *needle);
char* regex_str_slice(char*, int, int);
char* regex_url_fname(char*);

#endif /* REG_H */

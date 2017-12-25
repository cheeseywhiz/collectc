#ifndef REG_H
#define REG_H

char* strdup(char *str);

char* regex_match_one_subexpr(char*, char*, int);
int regex_contains(char *needle, char *haystack);
int regex_starts_with(char*, char*);
char* regex_str_slice(char*, int, int);
char* regex_url_fname(char*);

#endif /* REG_H */

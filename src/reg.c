#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAIL "fail314"

char* find_nocase(char *pattern, char *haystack) {
    char *needle;
    regex_t regbuf;
    regmatch_t pmatch[1];
    int ret_code;

    ret_code = regcomp(&regbuf, pattern, REG_EXTENDED | REG_ICASE);
    if (ret_code != 0) {
        // regfree(&regbuf);
        fprintf(stderr, "regcomp failed (%d)\n", ret_code);
        return FAIL;
    };

    ret_code = regexec(&regbuf, haystack, (size_t) 2, pmatch, 0);
    // regfree(&regbuf);
    if (ret_code != 0) {
        fprintf(stderr, "regexec failed (%d)\n", ret_code);
        return FAIL;
    };

    int start = (int) pmatch[1].rm_so;
    int end = (int) pmatch[1].rm_eo;
    int length = end - start;
    // int haylen = strlen(haystack);

    // for (int i = start; i < end; i++) {
    //     printf("%s\n", haystack[i]);
    // };

    return "hello";
};

int main(void) {
    find_nocase("length: (\\d*)", "HTTP/1.1 200 OK\r\nlength: 12345\r\n\r\n");
    return 0;
};

#include <stdlib.h>
#include <string.h>

char* str_slice(char *src, int start, int end) {
    char *dest;
    int dest_length = end - start;
    int src_length = strlen(src);

    if (dest_length > 0 && 0 <= start && start < src_length && 0 < end && end <= src_length) {
        dest = malloc(dest_length + 1);

        if (!dest) {
            return NULL;
        };

        bzero(dest, dest_length + 1);
        return strncpy(dest, src + start, dest_length);
    };

    return NULL;
};

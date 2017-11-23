#ifndef GET_H
#define GET_H

#include <stdlib.h>

struct response {
    char *type;
    char *content;
    size_t length;
};

void free_response(struct response*);

/* Abstraction for curl performance */
struct response* get_response(char*);

#endif /* GET_H */

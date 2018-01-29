#include <stdlib.h>

#include "test.h"
#include "ini.h"
#include "path.h"

SMALL_TEST test_parse(void) {
    SCORE_INIT();
    char *ini_str = path_contents("praw.ini");
    ju_json_t *ini = ini_parse(ini_str);
    ASSERT(ini->n_tokens == 21);
    ASSERT(ini->tokens[14].start == 153);
    ASSERT(ini->tokens[14].end == 166);
    ju_free(ini);
    free(ini_str);
    RETURN_SCORE();
}

BIG_TEST ini_test_main(void) {
    SCORE_INIT();
    SMALL_REPORT("ini_parse()", test_parse());
    RETURN_SCORE();
}

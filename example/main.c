#define META_PARSER_IMPLEMENTATION
#define META_LOG_CONSOLE
#include "meta_parser.h"

int main() {
    meta_parse_init();
    if (meta_parse("data.meta", "data.h") == 0) {
        printf("Code generation succeeded!\n");
    } else {
        printf("Error in code generation.\n");
    }
    return 0;
}
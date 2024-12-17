#define META_PARSER_IMPLEMENTATION
#include "meta_parser.h"

int main() {
    if (meta_parse("data.meta", "data.h") == 0) {
        printf("Code generation succeeded!\n");
    } else {
        printf("Error in code generation.\n");
    }
    return 0;
}
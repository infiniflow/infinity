//
// Created by jinhai on 23-3-8.
//

#include "parser_helper.h"
#include <cstring>
#include <cctype>

void
ParserHelper::ToLower(char* str) {
    size_t str_len = strlen(str);
    for(size_t i = 0; i < str_len; ++i) {
        str[i] = std::tolower(str[i]);
    }
}
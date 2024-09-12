// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "parser_helper.h"
#include <cctype>
#include <cstring>

void ParserHelper::ToLower(char *str) {
    if (str == nullptr) {
        return;
    }
    size_t str_len = strlen(str);
    for (size_t i = 0; i < str_len; ++i) {
        str[i] = std::tolower(str[i]);
    }
}

void ParserHelper::ToUpper(char *str) {
    if (str == nullptr) {
        return;
    }
    size_t str_len = strlen(str);
    for (size_t i = 0; i < str_len; ++i) {
        str[i] = std::toupper(str[i]);
    }
}

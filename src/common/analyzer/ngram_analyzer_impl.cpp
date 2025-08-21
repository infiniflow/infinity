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

module;

#include "string_utils.h"

module infinity_core:ngram_analyzer.impl;

import :ngram_analyzer;
import :term;
import :stemmer;
import :analyzer;
import :tokenizer;

namespace infinity {

bool NGramAnalyzer::NextInString(const char *data,
                                 size_t length,
                                 size_t *__restrict pos,
                                 size_t *__restrict token_start,
                                 size_t *__restrict token_length) {
    *token_start = *pos;
    *token_length = 0;
    size_t code_points = 0;
    for (; code_points < ngram_ && *token_start + *token_length < length; ++code_points) {
        if (std::isspace(data[*token_start + *token_length])) {
            *pos += UTF8SeqLength(static_cast<u8>(data[*pos]));
            *token_start = *pos;
            *token_length = 0;
            return true;
        }
        size_t sz = UTF8SeqLength(static_cast<u8>(data[*token_start + *token_length]));
        *token_length += sz;
    }
    *pos += UTF8SeqLength(static_cast<u8>(data[*pos]));
    return code_points == ngram_;
}

int NGramAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    size_t len = input.text_.length();
    if (len == 0)
        return 0;

    size_t cur = 0;
    size_t token_start = 0;
    size_t token_length = 0;
    size_t offset = input.word_offset_;
    while (cur < len && NextInString(input.text_.c_str(), len, &cur, &token_start, &token_length)) {
        if (token_length == 0)
            continue;
        func(data, input.text_.c_str() + token_start, token_length, offset, offset + token_length, false, 0);
        offset++;
    }

    return offset;
}

} // namespace infinity

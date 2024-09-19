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

export module standard_analyzer;

import stl;
import term;
import stemmer;
import analyzer;
import tokenizer;
import common_analyzer;

namespace infinity {
export class StandardAnalyzer : public CommonLanguageAnalyzer {
public:
    StandardAnalyzer() : CommonLanguageAnalyzer() {}

    ~StandardAnalyzer() {}

    void InitStemmer(Language language) { CommonLanguageAnalyzer::InitStemmer(language); }

protected:
    inline void Parse(const String &input) override {
        tokenizer_.Tokenize(input);
        local_offset_ = 0;
        ResetToken();
    }

    inline bool NextToken() override {
        if (DoNext()) {
            token_ = tokenizer_.GetToken();
            len_ = tokenizer_.GetLength();
            offset_ = get_char_offset_ ? tokenizer_.GetInputCursor() : local_offset_;
            local_offset_++;
            is_index_ = true;
            return true;
        } else {
            ResetToken();
            return false;
        }
    }

    bool DoNext() {
        while (tokenizer_.NextToken()) {
            if (!IsSpecialChar()) {
                return true;
            } else {
                continue;
            }
        }
        return false;
    }
    inline bool IsAlpha() override { return true; }

    inline bool IsSpecialChar() override { return tokenizer_.IsDelimiter(); }
};
} // namespace infinity
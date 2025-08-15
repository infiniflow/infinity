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

export module infinity_core:ngram_analyzer;

import :stl;
import :term;
import :stemmer;
import :analyzer;
import :tokenizer;

namespace infinity {
export class NGramAnalyzer : public Analyzer {
public:
    NGramAnalyzer(u32 ngram) : ngram_(ngram) {}

    virtual ~NGramAnalyzer() = default;

protected:
    int AnalyzeImpl(const Term &input, void *data, HookType func) override;

    bool NextInString(const char *data, size_t length, size_t *__restrict pos, size_t *__restrict token_start, size_t *__restrict token_length);

private:
    u32 ngram_;
};
} // namespace infinity

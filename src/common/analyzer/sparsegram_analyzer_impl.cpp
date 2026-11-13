// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

#include "common/utility/sparse_gram.h"

module infinity_core:sparsegram_analyzer.impl;

import :term;
import :analyzer;
import :sparsegram_analyzer;

import std;

namespace infinity {

int SparsegramAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    if (input.text_.empty()) {
        return 0;
    }
    SparseGramParams params;
    params.min_n = min_n_;
    params.max_n = max_n_;
    params.fold = fold_;

    // Positions are window ordinals, the same convention `ngram-N` uses. Gram
    // positions carry no meaning for a boolean narrowing, and nothing reads
    // them: the pattern is verified by the regular expression that runs after
    // the index scan.
    u32 offset = 0;
    ForEachSparseGram(input.text_, params, [&](std::string_view gram, u32, u32) {
        func(data, gram.data(), static_cast<u32>(gram.size()), offset, offset + 1, false, 0);
        ++offset;
    });
    return static_cast<int>(offset);
}

} // namespace infinity

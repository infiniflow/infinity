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

export module infinity_core:sparsegram_analyzer;

import :term;
import :stemmer;
import :analyzer;
import :tokenizer;

namespace infinity {

// Analyzer for regular expression acceleration.
//
// It emits the content-defined sparse grams of the whole value, unlike
// `ngram-N`, which restarts at every whitespace and can therefore never index a
// string that crosses a word boundary. Crossing those boundaries is what makes
// a multi word phrase usable as an index narrowing.
//
// A name of the form `sparsegram[-<min>[-<max>]][-fold]` configures it; see
// `common/utility/sparse_gram.h` for the selection rule and the reasoning
// behind the script-aware floors.
export class SparsegramAnalyzer : public Analyzer {
public:
    SparsegramAnalyzer(u32 min_n, u32 max_n, bool fold) : min_n_(min_n), max_n_(max_n), fold_(fold) {}

    ~SparsegramAnalyzer() override = default;

protected:
    int AnalyzeImpl(const Term &input, void *data, HookType func) override;

private:
    u32 min_n_;
    u32 max_n_;
    bool fold_;
};

} // namespace infinity

// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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
// limitations under the License.module;

module;

export module infinity_core:highlighter;

import :stl;
import :singleton;
import :aho_corasick;
import :analyzer;

namespace infinity {

export struct HighlightInfo {
    std::vector<std::string> query_terms_;
    std::string analyzer_;
};

export class Highlighter : public Singleton<Highlighter> {
public:
    Highlighter();

    void GetHighlightWithoutStemmer(const std::vector<std::string> &query, const std::string &raw_text, std::string &output);

    void GetHighlightWithStemmer(const std::vector<std::string> &query, const std::string &raw_text, std::string &output, Analyzer *analyzer);

private:
    AhoCorasick sentence_delimiter_;
};
}; // namespace infinity

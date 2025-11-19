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

export module infinity_core:highlighter;

import :singleton;
import :aho_corasick;
import :analyzer;

namespace infinity {

export struct HighlightInfo {
    std::string matching_text_;
    std::string analyzer_;
};

export class Highlighter : public Singleton<Highlighter> {
public:
    Highlighter(const std::string &pre_tag = "<em>", const std::string &post_tag = "</em>") : pre_tag_(pre_tag), post_tag_(post_tag) {}

    void GetHighlight(const std::string &matching_text, const std::string &raw_text, std::string &output, Analyzer *analyzer);

private:
    void GetASCIIWordHighlight(const std::vector<std::string> &query,
                               const std::string &raw_text,
                               std::vector<std::pair<size_t, size_t>> &matches,
                               Analyzer *analyzer);

    void GetUnicodeWordHighlight(const std::vector<std::string> &query, const std::string &raw_text, std::vector<std::pair<size_t, size_t>> &matches);

    void ApplyHighlights(const std::string &text, const std::vector<std::pair<size_t, size_t>> &matches, std::string &output);

    void FindASCIIWords(const std::string &pattern, const std::string &text, std::vector<std::pair<size_t, size_t>> &matches);

    std::string EscapeRegex(const std::string &input);

    std::vector<std::pair<size_t, size_t>> MergeMatches(const std::vector<std::pair<size_t, size_t>> &matches);

    bool IsASCIIWord(const std::string &word);

private:
    std::string pre_tag_;
    std::string post_tag_;
};
}; // namespace infinity

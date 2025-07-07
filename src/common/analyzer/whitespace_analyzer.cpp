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
// limitations under the License.

module;

#include <sstream>
#include <string>

module infinity_core;

import :stl;
import :term;
import :analyzer;
import :whitespace_analyzer;

namespace infinity {

WhitespaceAnalyzer::WhitespaceAnalyzer(const std::string_view delimiters) {
    delimiters_ = delimiters;
    std::sort(delimiters_.begin(), delimiters_.end());
    const auto last_unique = std::unique(delimiters_.begin(), delimiters_.end());
    delimiters_.erase(last_unique, delimiters_.end());
}

int WhitespaceAnalyzer::AnalyzeImpl(const Term &input, void *data, HookType func) {
    if (delimiters_.empty()) {
        // split by std::isspace()
        std::istringstream is(input.text_);
        std::string t;
        u32 offset = 0;
        while (is >> t) {
            func(data, t.data(), t.size(), offset++, 0, false, 0);
        }
        return 0;
    } else {
        // split by delimiters
        const std::string_view delimiters = delimiters_;
        const std::string_view input_text = input.text_;
        u32 search_start = 0;
        u32 offset = 0;
        while (search_start < input_text.size()) {
            const auto found = input_text.find_first_of(delimiters, search_start);
            if (found == std::string_view::npos) {
                func(data, input_text.data() + search_start, input_text.size() - search_start, offset++, 0, false, 0);
                break;
            }
            if (found > search_start) {
                func(data, input_text.data() + search_start, found - search_start, offset++, 0, false, 0);
            }
            search_start = found + 1;
        }
        return 0;
    }
}

} // namespace infinity

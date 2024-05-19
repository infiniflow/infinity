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

#include "search_options.h"
#include "spdlog/fmt/fmt.h"
#include <sstream>

namespace infinity {

SearchOptions::SearchOptions(const std::string &options_str) {
    options_.clear();
    if (options_str.empty()) {
        return;
    }
    size_t begin_idx = 0;
    size_t len = options_str.length();
    while (begin_idx < len) {
        size_t sem_idx = options_str.find_first_of(';', begin_idx);
        if (sem_idx == std::string::npos) {
            auto kv = ParseOption(options_str.substr(begin_idx));
            options_[kv.first] = kv.second;
            break;
        } else {
            auto kv = ParseOption(options_str.substr(begin_idx, sem_idx - begin_idx));
            options_[kv.first] = kv.second;
            begin_idx = sem_idx + 1;
        }
    }
}

bool SearchOptions::operator==(const SearchOptions &rhs) const {
    if (options_.size() != rhs.options_.size()) {
        return false;
    }
    for (auto &[k, v] : options_) {
        auto it = rhs.options_.find(k);
        if (it == rhs.options_.end() || it->second != v) {
            return false;
        }
    }
    return true;
}

std::string SearchOptions::ToString() {
    std::ostringstream oss;
//    size_t num_options = options_.size();
    auto it = options_.begin();
    auto end = options_.end();
    int cnt = 0;
    for (; it != end; ++it, ++cnt) {
        if (cnt != 0) {
            oss << ",";
        }
        oss << it->first << "=" << it->second;
    }
    return oss.str();
}

std::pair<std::string, std::string> SearchOptions::ParseOption(const std::string_view &option) {
    size_t eq_idx = option.find_first_of('=', 0);
    if (eq_idx == std::string::npos) {
        return std::make_pair(std::string(option), "");
    } else {
        std::string option_name(option.substr(0, eq_idx));
        std::string option_value(option.substr(eq_idx + 1));
        return std::make_pair(option_name, option_value);
    }
}

} // namespace infinity
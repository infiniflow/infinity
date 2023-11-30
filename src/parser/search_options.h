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

#pragma once
#include <map>
#include <string>
#include <utility>
namespace infinity {

class SearchOptions {
public:
    explicit SearchOptions(const std::string &options_str);
    virtual ~SearchOptions(){};
    bool operator==(const SearchOptions &other) const;
    bool operator!=(const SearchOptions &other) const { return !(*this == other); }

    std::string ToString();
    size_t size() const { return options_.size(); }
    std::map<std::string, std::string> options_;

private:
    std::pair<std::string, std::string> ParseOption(const std::string_view &option);
};

} // namespace infinity
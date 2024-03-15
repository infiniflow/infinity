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
#include <memory>
#include <string>
#include <vector>

namespace infinity {

struct QueryNode;

/**
 * Conducting the whole scanning and parsing.
 */
class SearchDriver {
public:
    SearchDriver(std::map<std::string, std::string> &field2analyzer, std::string &default_field)
        : field2analyzer_{field2analyzer}, default_field_{default_field} {}

    /**
     * parse a single query
     * @param query - a single query string
     */
    [[nodiscard]] std::unique_ptr<QueryNode> ParseSingle(const std::string &query) const;

    static std::unique_ptr<QueryNode> BuildQueryNodeByFieldAndTerms(const std::string &field, std::vector<std::string> &terms);

    void Analyze(const std::string &field, const std::string &text, std::vector<std::string> &terms) const;

    using AnalyzeFunc = void (*)(const std::string &analyzer_name, const std::string &text, std::vector<std::string> &terms);

    AnalyzeFunc analyze_func_{};

    /**
     * parsing options
     */
    const std::map<std::string, std::string> &field2analyzer_;
    const std::string &default_field_;

private:
    std::unique_ptr<QueryNode> ParseHelper(std::istream &stream) const;
};

} // namespace infinity

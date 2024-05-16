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
    SearchDriver(const std::map<std::string, std::string> &field2analyzer, const std::string &default_field)
        : field2analyzer_{field2analyzer}, default_field_{default_field} {}

    // used in PhysicalMatch
    [[nodiscard]] std::unique_ptr<QueryNode> ParseSingleWithFields(const std::string &fields_str, const std::string &query) const;

    // used in ParseSingleWithFields and unit_test
    [[nodiscard]] std::unique_ptr<QueryNode> ParseSingle(const std::string &query, const std::string *default_field_ptr = nullptr) const;

    // used in SearchParser in ParseSingle
    [[nodiscard]] std::unique_ptr<QueryNode> AnalyzeAndBuildQueryNode(const std::string &field, std::string &&text) const;

    /**
     * parsing options
     */
    const std::map<std::string, std::string> &field2analyzer_;
    const std::string &default_field_;
};

} // namespace infinity

//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

export module infinity_core:search_driver;

import :stl;
import :query_node;

namespace infinity {

export enum class FulltextQueryOperatorOption {
    kInfinitySyntax, // use parser's syntax
    kAnd,            // combine all terms with AND
    kOr,             // combine all terms with OR
};

/**
 * Conducting the whole scanning and parsing.
 */
export class SearchDriver {
public:
    SearchDriver(const Map<String, String> &field2analyzer,
                 const String &default_field,
                 const FulltextQueryOperatorOption operator_option = FulltextQueryOperatorOption::kInfinitySyntax)
        : field2analyzer_{field2analyzer}, default_field_{SearchDriver::Unescape(default_field)}, operator_option_(operator_option) {}

    // used in PhysicalMatch
    [[nodiscard]] UniquePtr<QueryNode> ParseSingleWithFields(const std::string &fields_str, const std::string &query) const;

    // used in ParseSingleWithFields and unit_test
    [[nodiscard]] UniquePtr<QueryNode> ParseSingle(const std::string &query, const std::string *default_field_ptr = nullptr) const;

    // used in SearchParser in ParseSingle. Assumes field and text are both unescaped.
    [[nodiscard]] UniquePtr<QueryNode>
    AnalyzeAndBuildQueryNode(const std::string &field, const std::string &text, bool from_quoted, unsigned long slop = 0) const;

    [[nodiscard]] static std::string Unescape(const std::string &text);

    /**
     * parsing options
     */
    const Map<String, String> &field2analyzer_;
    const String default_field_;
    const FulltextQueryOperatorOption operator_option_ = FulltextQueryOperatorOption::kInfinitySyntax;
};

// export using infinity::FulltextQueryOperatorOption;
// export using infinity::SearchDriver;

} // namespace infinity

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
class SearchParser;
class SearchScanner;

/**
 * Conducting the whole scanning and parsing.
 * WARNNING: Parse* are not thread-safe since they access the shared parsing options and result.
 */
class SearchDriver {
public:
    SearchDriver(const std::map<std::string, std::string> &field2analyzer_, const std::string &default_field_);

    /**
     * parse a stream - read and parse line by line
     * @param ist - std::istream&, valid input stream
     */
    // int ParseStream(std::istream &ist);

    /**
     * parse a single query
     * @param query - a single query string
     */
    int ParseSingle(const std::string &query);

    void Analyze(const std::string &field, const std::string &text, std::vector<std::string> &terms);

    static std::unique_ptr<QueryNode> BuildQueryNodeByFieldAndTerms(const std::string &field, std::vector<std::string> &terms);

    using AnalyzeFunc = void (*)(const std::string &analyzer_name, const std::string &text, std::vector<std::string> &terms);
    AnalyzeFunc analyze_func_{};

    /**
     * parsing result
     */
    std::unique_ptr<QueryNode> result_{};

    /**
     * parsing options
     */
    std::string default_field_;
    std::map<std::string, std::string> field2analyzer_;

private:
    int parse_helper(std::istream &stream);
    std::unique_ptr<SearchParser> parser_{};
    std::unique_ptr<SearchScanner> scanner_{};
};

} // namespace infinity

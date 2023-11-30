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

namespace irs {
class filter;
} // namespace irs

namespace infinity {

class QueryParser;
class QueryScanner;

/**
 * Conducting the whole scanning and parsing.
 * WARNNING: Parse* are not thread-safe since they access the shared parsing options and result.
 */
class QueryDriver {
public:
    QueryDriver(const std::map<std::string, std::string> &field2analyzer_, const std::string &default_field_);
    virtual ~QueryDriver();

    int ParseSingleWithFields(const std::string &fields_str, const std::string &query);

    /**
     * parse a stream - read and parse line by line
     * @param ist - std::istream&, valid input stream
     */
    int ParseStream(std::istream &ist);

    /**
     * parse a single query
     * @param query - a single query string
     */
    int ParseSingle(const std::string &query);

    void Analyze(const std::string &field, const std::string &text, std::vector<std::string> &terms);

    using AnalyzeFunc = void (*)(const std::string &analyzer_name, const std::string &text, std::vector<std::string> &terms);
    AnalyzeFunc analyze_func_{};

    /**
     * parsing result
     */
    std::unique_ptr<irs::filter> result = nullptr;

    /**
     * parsing options
     */
    std::string default_field;
    std::map<std::string, std::string> field2analyzer;

private:
    int parse_helper(std::istream &stream);
    std::unique_ptr<QueryParser> parser = nullptr;
    std::unique_ptr<QueryScanner> scanner = nullptr;
};

} // namespace infinity

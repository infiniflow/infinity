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

#include <cassert>
#include <istream>
#include <sstream>
#include <utility>

#include "query_node.h"
#include "search_driver.h"
#include "search_parser.h"
#include "search_scanner.h"

namespace infinity {
SearchDriver::SearchDriver(const std::map<std::string, std::string> &field2analyzer, const std::string &default_field)
    : default_field_{default_field}, field2analyzer_{field2analyzer} {}

int SearchDriver::ParseSingle(const std::string &query) {
    std::istringstream iss(query);
    if (!iss.good() && iss.eof()) {
        return -1;
    }
    int rc = parse_helper(iss);
    return rc;
}

void SearchDriver::Analyze(const std::string &field, const std::string &text, std::vector<std::string> &terms) {
    if (field.empty()) {
        terms.push_back(text);
        return;
    }
    auto it = field2analyzer_.find(field);
    if (it == field2analyzer_.end()) {
        terms.push_back(text);
        return;
    }
    const std::string &analyzer_name = it->second;
    if (analyzer_name.empty()) {
        terms.push_back(text);
        return;
    }
    analyze_func_(analyzer_name, text, terms);
}

std::unique_ptr<QueryNode> SearchDriver::BuildQueryNodeByFieldAndTerms(const std::string &field, std::vector<std::string> &terms) {
    if (terms.size() == 1) {
        auto result = std::make_unique<TermQueryNode>();
        result->term_ = std::move(terms[0]);
        result->column_ = field;
        return result;
    } else {
        auto result = std::make_unique<OrQueryNode>();
        for (std::string &term : terms) {
            auto subquery = std::make_unique<TermQueryNode>();
            subquery->term_ = std::move(term);
            subquery->column_ = field;
            result->Add(std::move(subquery));
        }
        return result;
    }
}

int SearchDriver::parse_helper(std::istream &stream) {
    try {
        scanner_ = std::make_unique<SearchScanner>(&stream);
        parser_ = std::make_unique<SearchParser>((*scanner_) /* scanner */, (*this) /* driver */);
    } catch (std::bad_alloc &ba) {
        std::cerr << "Failed to allocate: (" << ba.what() << "), exiting!!\n";
        return -1;
    }

    const int accept(0);
    if (parser_->parse() != accept) {
        return -1;
    }
    return 0;
}

} // namespace infinity

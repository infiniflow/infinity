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

std::pair<std::string, float> ParseField(const std::string_view &field) {
    size_t cap_idx = field.find_first_of('^', 0);
    if (cap_idx == std::string::npos) {
        return std::make_pair(std::string(field), 1.0F);
    } else {
        std::string field_name(field.substr(0, cap_idx));
        std::string_view field_boost = field.substr(cap_idx + 1);
        float boost = std::stof(std::string(field_boost));
        return std::make_pair(std::move(field_name), boost);
    }
}

void ParseFields(const std::string &fields_str, std::vector<std::pair<std::string, float>> &fields) {
    fields.clear();
    if (fields_str.empty())
        return;
    size_t begin_idx = 0;
    size_t len = fields_str.length();
    while (begin_idx < len) {
        size_t comma_idx = fields_str.find_first_of(',', begin_idx);
        if (comma_idx == std::string::npos) {
            auto field = ParseField(fields_str.substr(begin_idx));
            fields.push_back(field);
            break;
        } else {
            auto field = ParseField(fields_str.substr(begin_idx, comma_idx - begin_idx));
            fields.push_back(field);
            begin_idx = comma_idx + 1;
        }
    }
}

std::unique_ptr<QueryNode> SearchDriver::ParseSingleWithFields(const std::string &fields_str, const std::string &query) const {
    std::vector<std::pair<std::string, float>> fields;
    ParseFields(fields_str, fields);
    if (fields.empty()) {
        return ParseSingle(query);
    } else if (fields.size() == 1) {
        auto result = ParseSingle(query, &(fields[0].first));
        if (result) {
            result->MultiplyWeight(fields[0].second);
        }
        return result;
    } else {
        std::vector<std::unique_ptr<QueryNode>> or_children;
        for (auto &[default_field, boost] : fields) {
            auto sub_result = ParseSingle(query, &default_field);
            if (sub_result) {
                sub_result->MultiplyWeight(boost);
                or_children.emplace_back(std::move(sub_result));
            }
        }
        if (or_children.empty()) {
            return nullptr;
        } else if (or_children.size() == 1) {
            return std::move(or_children[0]);
        } else {
            auto node_or = std::make_unique<OrQueryNode>();
            node_or->children_ = std::move(or_children);
            return node_or;
        }
    }
}

std::unique_ptr<QueryNode> SearchDriver::ParseSingle(const std::string &query, const std::string *default_field_ptr) const {
    std::istringstream iss(query);
    if (!iss.good()) {
        return nullptr;
    }
    if (!default_field_ptr) {
        default_field_ptr = &default_field_;
    }
    return ParseHelper(iss, *default_field_ptr);
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

void SearchDriver::Analyze(const std::string &field, const std::string &text, std::vector<std::string> &terms) const {
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

std::unique_ptr<QueryNode> SearchDriver::ParseHelper(std::istream &stream, const std::string &default_field) const {
    std::unique_ptr<SearchScanner> scanner;
    std::unique_ptr<SearchParser> parser;
    std::unique_ptr<QueryNode> result;
    try {
        scanner = std::make_unique<SearchScanner>(&stream);
        parser = std::make_unique<SearchParser>(*scanner, *this, default_field, result);
    } catch (std::bad_alloc &ba) {
        std::cerr << "Failed to allocate: (" << ba.what() << "), exiting!!\n";
        return nullptr;
    }

    const int accept(0);
    if (parser->parse() != accept) {
        return nullptr;
    }
    return result;
}

} // namespace infinity

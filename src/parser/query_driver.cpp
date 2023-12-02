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

#include "query_driver.h"
#include "query_parser.h"
#include "query_scanner.h"
#include "search/boolean_filter.hpp"

namespace infinity {
QueryDriver::QueryDriver(const std::map<std::string, std::string> &field2analyzer_, const std::string &default_field_)
    : default_field(default_field_), field2analyzer(field2analyzer_) {}

QueryDriver::~QueryDriver() {}

static std::pair<std::string, float> ParseField(const std::string_view &field) {
    size_t cap_idx = field.find_first_of('^', 0);
    if (cap_idx == std::string::npos) {
        return std::make_pair(std::string(field), 1.0F);
    } else {
        std::string field_name(field.substr(0, cap_idx));
        std::string_view field_boost = field.substr(cap_idx + 1);
        float boost = std::stof(std::string(field_boost));
        return std::make_pair(field_name, boost);
    }
}

static void ParseFields(const std::string &fields_str, std::vector<std::pair<std::string, float>> &fields) {
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
    return;
}

int QueryDriver::ParseSingleWithFields(const std::string &fields_str, const std::string &query) {
    int rc = 0;
    std::vector<std::pair<std::string, float>> fields;
    ParseFields(fields_str, fields);
    if (fields.empty()) {
        rc = ParseSingle(query);
        if (rc != 0)
            return rc;
        return -1;
    } else if (fields.size() == 1) {
        auto it = fields.begin();
        default_field = it->first;
        rc = ParseSingle(query);
        if (rc != 0)
            return rc;
        result->boost(it->second);
    } else {
        auto flt = std::make_unique<irs::Or>();
        for (auto &field_boost : fields) {
            default_field = field_boost.first;
            rc = ParseSingle(query);
            if (rc != 0)
                return rc;
            result->boost(field_boost.second);
            flt->add(std::move(result));
        }
        result = std::move(flt);
    }
    return 0;
}

int QueryDriver::ParseStream(std::istream &ist) {
    // read and parse line by line, ignoring empty lines and comments
    std::string line;
    while (std::getline(ist, line)) {
        size_t firstNonBlank = line.find_first_not_of(" \t");
        if (firstNonBlank == std::string::npos || line[firstNonBlank] == '#') {
            continue;
        }
        line = line.substr(firstNonBlank);
        std::cerr << "---query: ###" << line << "###" << std::endl;
        int rc = ParseSingle(line);
        if (rc != 0) {
            std::cerr << "---failed" << std::endl;
            return rc;
        } else {
            std::cerr << "---accepted" << std::endl;
        }
    }
    return 0;
}

int QueryDriver::ParseSingle(const std::string &query) {
    std::istringstream iss(query);
    if (!iss.good() && iss.eof()) {
        return -1;
    }
    int rc = parse_helper(iss);
    return rc;
}

int QueryDriver::parse_helper(std::istream &stream) {
    try {
        scanner = std::make_unique<QueryScanner>(&stream);
        parser = std::make_unique<QueryParser>((*scanner) /* scanner */, (*this) /* driver */);
    } catch (std::bad_alloc &ba) {
        std::cerr << "Failed to allocate: (" << ba.what() << "), exiting!!\n";
        return -1;
    }

    const int accept(0);
    if (parser->parse() != accept) {
        return -1;
    }
    return 0;
}

void QueryDriver::Analyze(const std::string &field, const std::string &text, std::vector<std::string> &terms) {
    if (field.empty()) {
        terms.push_back(text);
        return;
    }
    auto it = field2analyzer.find(field);
    if (it == field2analyzer.end()) {
        terms.push_back(text);
        return;
    }
    std::string &analyzer_name = it->second;
    if (analyzer_name.empty()) {
        terms.push_back(text);
        return;
    }
    analyze_func_(analyzer_name, text, terms);
}

} // namespace infinity

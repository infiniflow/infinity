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

#include "search_parser.h"
#include "spdlog/fmt/fmt.h"
#include <sstream>

namespace infinity {

void SearchParser::ParseFields(const std::string &fields_str, std::vector<std::pair<std::string, float>> &fields) {
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

std::string SearchParser::FieldsToString(const std::vector<std::pair<std::string, float>> &fields) {
    std::ostringstream oss;
    size_t num_fields = fields.size();
    auto field_to_str = [](const std::pair<std::string, float> &field) {
        if (std::abs(field.second - 1.0) < 0.001)
            return field.first;
        return fmt::format("{}^{}", field.first, field.second);
    };
    if (num_fields == 0) {
        oss << "''";
    } else if (num_fields == 1) {
        oss << field_to_str(fields[0]);
    } else {
        oss << "'" << field_to_str(fields[0]);
        for (size_t i = 1; i < num_fields; i++) {
            oss << "," << field_to_str(fields[i]);
        }
        oss << "'";
    }
    return oss.str();
}

void SearchParser::ParseOptions(const std::string &options_str, std::vector<std::pair<std::string, std::string>> &options) {
    options.clear();
    if (options_str.empty())
        return;
    size_t begin_idx = 0;
    size_t len = options_str.length();
    while (begin_idx < len) {
        size_t sem_idx = options_str.find_first_of(';', begin_idx);
        if (sem_idx == std::string::npos) {
            auto field = ParseOption(options_str.substr(begin_idx));
            options.push_back(field);
            break;
        } else {
            auto field = ParseOption(options_str.substr(begin_idx, sem_idx - begin_idx));
            options.push_back(field);
            begin_idx = sem_idx + 1;
        }
    }
    return;
}

std::string SearchParser::OptionsToString(const std::vector<std::pair<std::string, std::string>> &options) {
    std::ostringstream oss;
    oss << "'";
    size_t num_options = options.size();
    if (!options.empty()) {
        oss << options[0].first << "=" << options[0].second;
        for (size_t i = 1; i < num_options; i++) {
            oss << "," << options[i].first << "=" << options[i].second;
        }
    }
    oss << "'";
    return oss.str();
}

std::pair<std::string, float> SearchParser::ParseField(const std::string_view &field) {
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

std::pair<std::string, std::string> SearchParser::ParseOption(const std::string_view &option) {
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
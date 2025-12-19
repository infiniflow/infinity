// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:json_manager.impl;

import :json_manager;

namespace infinity {

// " --> ""
std::string JsonManager::escapeQuotes(const std::string &input) {
    std::regex pattern("\"");
    return std::regex_replace(input, pattern, "\"\"");
}

// \" --> "
std::string JsonManager::unescapeQuotes(const std::string &input) {
    std::regex pattern("\\\"");
    return std::regex_replace(input, pattern, "\"");
}

bool JsonManager::valid_json(const std::string &valid_json) { return JsonTypeDef::accept(valid_json); }

JsonTypeDef JsonManager::parse(std::string &json_str) {
    try {
        return JsonTypeDef::parse(json_str);
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::parse error: {}", e.what()));
    }
    return {};
}

JsonTypeDef JsonManager::from_bson(const std::vector<uint8_t> &bson_data) {
    try {
        return JsonTypeDef::from_bson(bson_data);
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::from_bson error: {}", e.what()));
    }
    return {};
}

std::string JsonManager::dump(const JsonTypeDef &json_obj) {
    try {
        return json_obj.dump();
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::dump error: {}", e.what()));
    }
    return {};
}

std::vector<uint8_t> JsonManager::to_bson(const JsonTypeDef &json_obj) {
    try {
        return JsonTypeDef::to_bson(json_obj);
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::to_bson error: {}", e.what()));
    }
    return {};
}

bool JsonManager::check_json_path(const std::string &json_path) {
    if (json_path.empty() || json_path[0] != '$') {
        return false;
    }
    return true;
}

bool JsonManager::check_json_path(const std::string_view &json_path) {
    if (json_path.empty() || json_path[0] != '$') {
        return false;
    }
    return true;
}

std::tuple<bool, std::vector<std::string>> JsonManager::get_json_tokens(const std::string &json_path) {
    std::vector<std::string> json_tokens;

    if (!JsonManager::check_json_path(json_path)) {
        return {false, {}};
    }

    std::string_view remaining = std::string_view(json_path).substr(1);
    size_t start = 0;
    size_t end = remaining.find('.');

    while (start < remaining.length()) {
        auto token_end = (end == std::string_view::npos) ? remaining.length() : end;

        if (start < token_end) {
            json_tokens.emplace_back(remaining.substr(start, token_end - start));
        }

        if (end == std::string_view::npos)
            break;

        start = end + 1;
        end = remaining.find('.', start);
    }

    return {true, std::move(json_tokens)};
}

std::tuple<bool, std::string> JsonManager::json_extract(const JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef current = data;
    for (const auto &token : tokens) {
        if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {true, "null"};
        }
    }
    return {false, current.dump()};
}

std::tuple<bool, IntegerT> JsonManager::json_extract_int(const JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef current = data;
    for (const auto &token : tokens) {
        if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {true, 0};
        }
    }

    if (current.is_number_integer()) {
        return {false, current.get<int>()};
    } else {
        return {true, 0};
    }
}

std::tuple<bool, DoubleT> JsonManager::json_extract_double(const JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef current = data;
    for (const auto &token : tokens) {
        if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {true, 0.0};
        }
    }

    if (current.is_number_float()) {
        return {false, current.get<double>()};
    } else {
        return {true, 0.0};
    }
}

std::tuple<bool, BooleanT> JsonManager::json_extract_bool(const JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef current = data;
    for (const auto &token : tokens) {
        if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {true, false};
        }
    }

    if (current.is_boolean()) {
        return {false, current.get<bool>()};
    } else {
        return {true, false};
    }
}

std::tuple<bool, BooleanT> JsonManager::json_extract_is_null(const JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef current = data;
    for (const auto &token : tokens) {
        if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {true, false};
        }
    }

    if (current.is_null()) {
        return {false, true};
    } else {
        return {false, false};
    }
}

} // namespace infinity
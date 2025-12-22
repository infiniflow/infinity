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

bool JsonManager::valid_json(const std::string &valid_json) {
    try {
        return JsonTypeDef::accept(valid_json);
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::valid_json error: {}", e.what()));
    }
    return false;
}

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
        auto tmp_bson_data = JsonTypeDef::from_bson(bson_data);
        return tmp_bson_data["data"];
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
        JsonTypeDef wrapper = {{"data", json_obj}};
        return JsonTypeDef::to_bson(wrapper);
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
    size_t pos = 0;
    while (pos < remaining.length()) {
        if (remaining[pos] == '.') {
            pos++;
            continue;
        }

        if (remaining[pos] == '[') {
            size_t bracket_end = remaining.find(']', pos);
            if (bracket_end == std::string_view::npos) {
                return {false, {}};
            }

            std::string_view index_content = remaining.substr(pos + 1, bracket_end - pos - 1);
            if (index_content.empty()) {
                return {false, {}};
            }

            bool is_numeric_index = true;
            for (char c : index_content) {
                if (!std::isdigit(c)) {
                    is_numeric_index = false;
                    break;
                }
            }

            if (is_numeric_index) {
                json_tokens.emplace_back(index_content);
            } else {
                return {false, {}};
            }

            pos = bracket_end + 1;
        } else {
            size_t token_end = pos;
            while (token_end < remaining.length()) {
                if (remaining[token_end] == '.' || remaining[token_end] == '[') {
                    break;
                }
                token_end++;
            }

            if (token_end > pos) {
                std::string_view token = remaining.substr(pos, token_end - pos);
                if (!token.empty()) {
                    json_tokens.emplace_back(token);
                }
                pos = token_end;
            } else {
                pos++;
            }
        }
    }

    return {true, std::move(json_tokens)};
}

std::tuple<bool, std::string> JsonManager::json_extract(JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef &current = data;
    for (const auto &token : tokens) {
        if (current.is_array() && token.find_first_not_of("0123456789") == std::string::npos) {
            try {
                size_t index = std::stoul(token);
                if (index < current.size()) {
                    current = current[index];
                } else {
                    return {false, "null"};
                }
            } catch (const std::exception &e) {
                return {false, "null"};
            }
        } else if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {false, "null"};
        }
    }
    return {true, current.dump()};
}

std::tuple<bool, IntegerT> JsonManager::json_extract_int(JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef &current = data;
    for (const auto &token : tokens) {
        if (current.is_array() && token.find_first_not_of("0123456789") == std::string::npos) {
            try {
                size_t index = std::stoul(token);
                if (index < current.size()) {
                    current = current[index];
                } else {
                    return {false, 0};
                }
            } catch (const std::exception &e) {
                return {false, 0};
            }
        } else if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {false, 0};
        }
    }

    if (current.is_number_integer()) {
        return {true, current.get<int>()};
    } else {
        return {false, 0};
    }
}

std::tuple<bool, DoubleT> JsonManager::json_extract_double(JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef &current = data;
    for (const auto &token : tokens) {
        if (current.is_array() && token.find_first_not_of("0123456789") == std::string::npos) {
            try {
                size_t index = std::stoul(token);
                if (index < current.size()) {
                    current = current[index];
                } else {
                    return {false, 0.0};
                }
            } catch (const std::exception &e) {
                return {false, 0.0};
            }
        } else if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {false, 0.0};
        }
    }

    if (current.is_number_float()) {
        return {true, current.get<double>()};
    } else {
        return {false, 0.0};
    }
}

std::tuple<bool, BooleanT> JsonManager::json_extract_bool(JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef &current = data;
    for (const auto &token : tokens) {
        if (current.is_array() && token.find_first_not_of("0123456789") == std::string::npos) {
            try {
                size_t index = std::stoul(token);
                if (index < current.size()) {
                    current = current[index];
                } else {
                    return {false, false};
                }
            } catch (const std::exception &e) {
                return {false, false};
            }
        } else if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {false, false};
        }
    }

    if (current.is_boolean()) {
        return {true, current.get<bool>()};
    } else {
        return {false, false};
    }
}

std::tuple<bool, BooleanT> JsonManager::json_extract_is_null(JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef &current = data;
    for (const auto &token : tokens) {
        if (current.is_array() && token.find_first_not_of("0123456789") == std::string::npos) {
            try {
                size_t index = std::stoul(token);
                if (index < current.size()) {
                    current = current[index];
                } else {
                    return {false, false};
                }
            } catch (const std::exception &e) {
                return {false, false};
            }
        } else if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {false, false};
        }
    }

    if (current.is_null()) {
        return {true, true};
    } else {
        return {true, false};
    }
}

std::tuple<bool, BooleanT> JsonManager::json_extract_exists_path(JsonTypeDef &data, const std::vector<std::string> &tokens) {
    JsonTypeDef &current = data;
    for (const auto &token : tokens) {
        if (current.is_array() && token.find_first_not_of("0123456789") == std::string::npos) {
            try {
                size_t index = std::stoul(token);
                if (index < current.size()) {
                    current = current[index];
                } else {
                    return {true, false};
                }
            } catch (const std::exception &e) {
                return {true, false};
            }
        } else if (current.is_object() && current.contains(token)) {
            current = current[token];
        } else {
            return {true, false};
        }
    }
    return {true, true};
}

} // namespace infinity
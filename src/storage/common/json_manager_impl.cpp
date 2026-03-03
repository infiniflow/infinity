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
import std.compat;

namespace infinity {

// " --> ""
std::string JsonManager::escapeQuotes(const std::string &input) {
    if (input.empty()) {
        return input;
    }

    // Count quotes to escape
    size_t quote_count = 0;
    for (char c : input) {
        if (c == '"') {
            ++quote_count;
        }
    }

    if (quote_count == 0) {
        return input;
    }

    // Reserve space for the result
    std::string result;
    result.reserve(input.size() + quote_count);

    // Replace " with ""
    for (char c : input) {
        if (c == '"') {
            result.append("\"\"");
        } else {
            result.push_back(c);
        }
    }

    return result;
}

// "" --> "
std::string JsonManager::unescapeQuotes(const std::string &input) {
    if (input.empty()) {
        return input;
    }

    std::string result;
    result.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {
        if (i + 1 < input.size() && input[i] == '"' && input[i + 1] == '"') {
            result.push_back('"');
            i += 2;
        } else {
            result.push_back(input[i]);
            ++i;
        }
    }

    return result;
}

bool JsonManager::valid_json(const std::string &json_str) {
    if (json_str.empty()) {
        return false;
    }
    try {
        return JsonTypeDef::accept(json_str);
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::valid_json error: {}", e.what()));
    } catch (...) {
        LOG_TRACE("JsonManager::valid_json unknown error");
    }
    return false;
}

JsonTypeDef JsonManager::parse(const std::string &json_str) {
    try {
        return JsonTypeDef::parse(json_str);
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::parse error: {}", e.what()));
    } catch (...) {
        LOG_TRACE("JsonManager::parse unknown error");
    }
    return {};
}

std::unique_ptr<JsonTypeDef> JsonManager::from_bson(const std::vector<uint8_t> &bson_data) {
    try {
        auto tmp_bson_data = JsonTypeDef::from_bson(bson_data);
        return std::make_unique<JsonTypeDef>(std::move(tmp_bson_data["data"]));
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::from_bson error: {}", e.what()));
    } catch (...) {
        LOG_TRACE("JsonManager::from_bson unknown error");
    }
    return nullptr;
}

std::unique_ptr<JsonTypeDef> JsonManager::from_bson(const uint8_t *bson_data, size_t len) {
    try {
        auto tmp_bson_data = JsonTypeDef::from_bson(bson_data, len);
        return std::make_unique<JsonTypeDef>(std::move(tmp_bson_data["data"]));
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::from_bson error: {}", e.what()));
    } catch (...) {
        LOG_TRACE("JsonManager::from_bson unknown error");
    }
    return nullptr;
}

std::string JsonManager::dump(const JsonTypeDef &json_obj) {
    try {
        return json_obj.dump();
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::dump error: {}", e.what()));
    } catch (...) {
        LOG_TRACE("JsonManager::dump unknown error");
    }
    return {};
}

std::vector<uint8_t> JsonManager::to_bson(const JsonTypeDef &json_obj) {
    try {
        JsonTypeDef wrapper = {{"data", json_obj}};
        return JsonTypeDef::to_bson(wrapper);
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::to_bson error: {}", e.what()));
    } catch (...) {
        LOG_TRACE("JsonManager::to_bson unknown error");
    }
    return {};
}

std::vector<uint8_t> JsonManager::to_bson(JsonTypeDef &&json_obj) {
    try {
        JsonTypeDef wrapper;
        wrapper["data"] = std::move(json_obj);
        return JsonTypeDef::to_bson(wrapper);
    } catch (const JsonTypeDef::parse_error &e) {
        LOG_TRACE(fmt::format("JsonManager::to_bson error: {}", e.what()));
    } catch (...) {
        LOG_TRACE("JsonManager::to_bson unknown error");
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

std::tuple<bool, std::vector<JsonTokenInfo>> JsonManager::get_json_tokens(const std::span<const char> &json_path) {
    std::string_view path_view(json_path.data(), json_path.size());
    if (!JsonManager::check_json_path(path_view)) {
        return {false, {}};
    }

    std::vector<std::pair<JsonType, std::string>> json_tokens;
    std::string_view remaining = path_view.substr(1);
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
                json_tokens.emplace_back(std::make_pair(JsonType::kJsonArray, index_content));
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
                    json_tokens.emplace_back(std::make_pair(JsonType::kJsonObject, token));
                }
                pos = token_end;
            } else {
                pos++;
            }
        }
    }

    return {true, json_tokens};
}

std::tuple<bool, std::vector<std::pair<JsonType, std::string>>> JsonManager::get_json_tokens(const std::string &json_path) {
    std::span<const char> path_span(json_path.data(), json_path.size());
    return get_json_tokens(path_span);
}

std::tuple<bool, std::string> JsonManager::json_extract(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens) {
    const JsonTypeDef *current = &data;
    for (const auto &token : tokens) {
        const auto &token_type = token.first;
        const auto &token_value = token.second;
        if (current->is_array() && token_type == JsonType::kJsonArray) {
            try {
                size_t index = std::stoul(token_value);
                if (index < current->size()) {
                    current = &(*current)[index];
                } else {
                    return {false, "null"};
                }
            } catch (const std::exception &) {
                return {false, "null"};
            }
        } else if (current->is_object() && token_type == JsonType::kJsonObject && current->contains(token_value)) {
            current = &(*current)[token_value];
        } else {
            return {false, "null"};
        }
    }
    return {true, current->dump()};
}

std::tuple<bool, BigIntT> JsonManager::json_extract_int(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens) {
    const JsonTypeDef *current = &data;
    for (const auto &token : tokens) {
        const auto &token_type = token.first;
        const auto &token_value = token.second;
        if (current->is_array() && token_type == JsonType::kJsonArray) {
            try {
                size_t index = std::stoul(token_value);
                if (index < current->size()) {
                    current = &(*current)[index];
                } else {
                    return {false, 0};
                }
            } catch (const std::exception &) {
                return {false, 0};
            }
        } else if (current->is_object() && token_type == JsonType::kJsonObject && current->contains(token_value)) {
            current = &(*current)[token_value];
        } else {
            return {false, 0};
        }
    }

    if (current->is_number_integer()) {
        return {true, current->get<BigIntT>()};
    }
    return {false, 0};
}

std::tuple<bool, DoubleT> JsonManager::json_extract_double(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens) {
    const JsonTypeDef *current = &data;
    for (const auto &token : tokens) {
        const auto &token_type = token.first;
        const auto &token_value = token.second;
        if (current->is_array() && token_type == JsonType::kJsonArray) {
            try {
                size_t index = std::stoul(token_value);
                if (index < current->size()) {
                    current = &(*current)[index];
                } else {
                    return {false, 0.0};
                }
            } catch (const std::exception &) {
                return {false, 0.0};
            }
        } else if (current->is_object() && token_type == JsonType::kJsonObject && current->contains(token_value)) {
            current = &(*current)[token_value];
        } else {
            return {false, 0.0};
        }
    }

    if (current->is_number()) {
        return {true, current->get<double>()};
    }
    return {false, 0.0};
}

std::tuple<bool, BooleanT> JsonManager::json_extract_bool(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens) {
    const JsonTypeDef *current = &data;
    for (const auto &token : tokens) {
        const auto &token_type = token.first;
        const auto &token_value = token.second;
        if (current->is_array() && token_type == JsonType::kJsonArray) {
            try {
                size_t index = std::stoul(token_value);
                if (index < current->size()) {
                    current = &(*current)[index];
                } else {
                    return {false, false};
                }
            } catch (const std::exception &) {
                return {false, false};
            }
        } else if (current->is_object() && token_type == JsonType::kJsonObject && current->contains(token_value)) {
            current = &(*current)[token_value];
        } else {
            return {false, false};
        }
    }

    if (current->is_boolean()) {
        return {true, current->get<bool>()};
    }
    return {false, false};
}

std::tuple<bool, BooleanT> JsonManager::json_extract_is_null(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens) {
    const JsonTypeDef *current = &data;
    for (const auto &token : tokens) {
        const auto &token_type = token.first;
        const auto &token_value = token.second;
        if (current->is_array() && token_type == JsonType::kJsonArray) {
            try {
                size_t index = std::stoul(token_value);
                if (index < current->size()) {
                    current = &(*current)[index];
                } else {
                    return {false, false};
                }
            } catch (const std::exception &) {
                return {false, false};
            }
        } else if (current->is_object() && token_type == JsonType::kJsonObject && current->contains(token_value)) {
            current = &(*current)[token_value];
        } else {
            return {false, false};
        }
    }

    return {true, current->is_null()};
}

std::tuple<bool, BooleanT> JsonManager::json_extract_exists_path(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens) {
    const JsonTypeDef *current = &data;
    for (const auto &token : tokens) {
        const auto &token_type = token.first;
        const auto &token_value = token.second;
        if (current->is_array() && token_type == JsonType::kJsonArray) {
            try {
                size_t index = std::stoul(token_value);
                if (index < current->size()) {
                    current = &(*current)[index];
                } else {
                    return {true, false};
                }
            } catch (const std::exception &) {
                return {true, false};
            }
        } else if (current->is_object() && token_type == JsonType::kJsonObject && current->contains(token_value)) {
            current = &(*current)[token_value];
        } else {
            return {true, false};
        }
    }
    return {true, true};
}

BooleanT JsonManager::json_contains(const JsonTypeDef &data, const std::string &token) {
    if (!data.is_array()) {
        return false;
    }

    JsonTypeDef parsed_token;
    bool parse_success = false;

    try {
        parsed_token = JsonTypeDef::parse(token);
        parse_success = true;
    } catch (...) {
        // If parsing fails, treat token as a literal string value
    }

    if (!parse_success) {
        return false;
    }

    switch (parsed_token.type()) {
        case JsonValueType::string: {
            const auto &token_value = parsed_token.get_ref<const std::string &>();
            return std::any_of(data.begin(), data.end(), [&token_value](const JsonTypeDef &element) {
                return element.is_string() && element.get_ref<const std::string &>() == token_value;
            });
        }
        case JsonValueType::number_integer: {
            const auto token_value = parsed_token.get<int64_t>();
            return std::any_of(data.begin(), data.end(), [token_value](const JsonTypeDef &element) {
                return element.is_number_integer() && element.get<int64_t>() == token_value;
            });
        }
        case JsonValueType::number_float: {
            const auto token_value = parsed_token.get<double>();
            return std::any_of(data.begin(), data.end(), [token_value](const JsonTypeDef &element) {
                return element.is_number_float() && std::abs(element.get<double>() - token_value) < 1e-10;
            });
        }
        case JsonValueType::boolean: {
            const auto token_value = parsed_token.get<bool>();
            return std::any_of(data.begin(), data.end(), [&token_value](const JsonTypeDef &element) {
                return element.is_boolean() && element.get<bool>() == token_value;
            });
        }
        case JsonValueType::null: {
            return std::any_of(data.begin(), data.end(), [](const JsonTypeDef &element) { return element.is_null(); });
        }
        default: {
            const std::string token_value = parsed_token.dump();
            return std::any_of(data.begin(), data.end(), [&token_value](const JsonTypeDef &element) { return element.dump() == token_value; });
        }
    }
}

std::tuple<size_t, std::vector<std::string>> JsonManager::json_unnest(const JsonTypeDef &data) {
    std::vector<std::string> result;
    if (data.is_array()) {
        result.reserve(data.size());
        for (const auto &element : data) {
            result.emplace_back(element.dump());
        }
    } else {
        result.emplace_back(data.dump());
    }
    return {result.size(), std::move(result)};
}

} // namespace infinity
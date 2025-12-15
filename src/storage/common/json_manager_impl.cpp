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
std::string JsonManager::escapeQuotes(const std::string& input) {
    std::regex pattern("\"");
    return std::regex_replace(input, pattern, "\"\"");
}

// \" --> "
std::string JsonManager::unescapeQuotes(const std::string& input) {
    std::regex pattern("\\\"");
    return std::regex_replace(input, pattern, "\"");
}

bool JsonManager::valid_json(const std::string &valid_json) { return JsonType::accept(valid_json); }

JsonType JsonManager::parse(std::string &json_str) {
    try {
        return JsonType::parse(json_str);
    } catch (const JsonType::parse_error& e) {
        LOG_INFO(fmt::format("JsonManager::parse error: {}", e.what()));
    }

    return{};
}

JsonType JsonManager::from_bson(const std::vector<uint8_t> &bson_data) {
    try {
        return JsonType::from_bson(bson_data);
    } catch (const JsonType::parse_error& e) {
        LOG_INFO(fmt::format("JsonManager::from_bson error: {}", e.what()));
    }

    return{};
}

std::string JsonManager::dump(const JsonType &json_obj) {
    try {
        return json_obj.dump();;
    } catch (const JsonType::parse_error& e) {
        LOG_INFO(fmt::format("JsonManager::dump error: {}", e.what()));
    }

    return{};
}

std::vector<uint8_t> JsonManager::to_bson(const JsonType &json_obj) {
    try {
        return JsonType::to_bson(json_obj);
    } catch (const JsonType::parse_error& e) {
        LOG_INFO(fmt::format("JsonManager::to_bson error: {}", e.what()));
    }

    return{};
}

} // namespace infinity
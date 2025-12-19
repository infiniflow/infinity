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

module;

export module infinity_core:json_manager;

import :logger;
import :value;

import std.compat;
import third_party;

namespace infinity {

export using JsonTypeDef = nlohmann::json;

export class JsonManager {
public:
    static std::string escapeQuotes(const std::string &input);
    static std::string unescapeQuotes(const std::string &input);
    static bool valid_json(const std::string &json_str);
    static JsonTypeDef parse(std::string &json_str);
    static JsonTypeDef from_bson(const std::vector<uint8_t> &bson_data);
    static std::string dump(const JsonTypeDef &json_obj);
    static std::vector<uint8_t> to_bson(const JsonTypeDef &json_obj);

    static bool check_json_path(const std::string &json_path);
    static bool check_json_path(const std::string_view &json_path);
    static std::tuple<bool, std::vector<std::string>> get_json_tokens(const std::string &json_path);

    static std::tuple<bool, std::string> json_extract(const JsonTypeDef &data, const std::vector<std::string> &tokens);
    static std::tuple<bool, IntegerT> json_extract_int(const JsonTypeDef &data, const std::vector<std::string> &tokens);
    static std::tuple<bool, DoubleT> json_extract_double(const JsonTypeDef &data, const std::vector<std::string> &tokens);
    static std::tuple<bool, BooleanT> json_extract_bool(const JsonTypeDef &data, const std::vector<std::string> &tokens);
    static std::tuple<bool, BooleanT> json_extract_is_null(const JsonTypeDef &data, const std::vector<std::string> &tokens);
};

} // namespace infinity

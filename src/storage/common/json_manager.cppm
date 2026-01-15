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
export enum class JsonType : uint8_t { kInvalid, kJsonObject, kJsonArray };
export using JsonTokenInfo = std::pair<JsonType, std::string>;

export class JsonManager {
public:
    static std::string escapeQuotes(const std::string &input);
    static std::string unescapeQuotes(const std::string &input);
    static bool valid_json(const std::string &json_str);
    static JsonTypeDef parse(const std::string &json_str);

    // json --> string
    static std::string dump(const JsonTypeDef &json_obj);

    // bson --> json
    static JsonTypeDef from_bson(const std::vector<uint8_t> &bson_data);
    static JsonTypeDef from_bson(const unit8_t *bson_data, size_t len);

    // json --> bson
    static std::vector<uint8_t> to_bson(const JsonTypeDef &json_obj);

    static bool check_json_path(const std::string &json_path);
    static bool check_json_path(const std::string_view &json_path);

    // Parse json path
    static std::tuple<bool, std::vector<JsonTokenInfo>> get_json_tokens(const std::string &json_path);

    /* extract json
     * return: arg1: is_null, arg2: result
     */
    static std::tuple<bool, std::string> json_extract(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens);
    static std::tuple<bool, IntegerT> json_extract_int(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens);
    static std::tuple<bool, DoubleT> json_extract_double(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens);
    static std::tuple<bool, BooleanT> json_extract_bool(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens);
    static std::tuple<bool, BooleanT> json_extract_is_null(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens);
    static std::tuple<bool, BooleanT> json_extract_exists_path(const JsonTypeDef &data, const std::vector<JsonTokenInfo> &tokens);

    static BooleanT json_contains(const JsonTypeDef &data, const std::string &token);

    static std::tuple<size_t, std::vector<std::string>> json_unnest(const JsonTypeDef &data);
};

} // namespace infinity

// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace infinity {

using JsonTypeDef = nlohmann::json;

struct FlattenedTerm {
    std::string term_;
};

class JsonFlattener {
public:
    // Flatten a BSON document into multiple terms
    static std::vector<FlattenedTerm> FlattenDocument(const uint8_t *bson_data, size_t bson_length);

    // Flatten from nlohmann::json
    static std::vector<FlattenedTerm> FlattenJson(const JsonTypeDef &json_obj);

private:
    static void FlattenRecursive(const JsonTypeDef &json_value, const std::string &current_path, std::vector<FlattenedTerm> &terms);

    // Helper to encode integer with zero-padding
    static std::string EncodeInteger(int64_t value);

    // Helper to encode double with zero-padding
    static std::string EncodeDouble(double value);

    // Helper to encode path
    static std::string EncodePath(const std::string &path);
};

} // namespace infinity
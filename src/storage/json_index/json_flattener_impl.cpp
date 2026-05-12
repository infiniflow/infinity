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

module infinity_core:json_flattener.impl;

import :json_flattener;

import :json_manager;
import :infinity_exception;

import std;

namespace infinity {

std::string JsonFlattener::EncodeInteger(int64_t value) {
    // Flip the sign bit so signed integer order matches lexicographic order.
    uint64_t sortable_bits = std::bit_cast<uint64_t>(value) ^ (uint64_t{1} << 63);

    char buf[17];
    int len = std::snprintf(buf, sizeof(buf), "%016llX", static_cast<unsigned long long>(sortable_bits));
    return std::string(buf, len);
}

std::string JsonFlattener::EncodeDouble(double value) {
    // Encode doubles into a sortable IEEE-754 key so lexicographic term order
    // matches numeric order without losing precision at strict range boundaries.
    if (value == 0.0) {
        value = 0.0;
    }
    uint64_t bits = std::bit_cast<uint64_t>(value);
    const uint64_t sortable_bits = (bits & (uint64_t{1} << 63)) ? ~bits : (bits ^ (uint64_t{1} << 63));

    char buf[17];
    int len = std::snprintf(buf, sizeof(buf), "%016llX", static_cast<unsigned long long>(sortable_bits));
    return std::string(buf, len);
}

std::string JsonFlattener::EncodePath(const std::string &path) { return path; }

void JsonFlattener::FlattenRecursive(const JsonTypeDef &json_value, const std::string &current_path, std::vector<FlattenedTerm> &terms) {
    // Helper lambda to add a scalar value as a term
    auto AddScalarTerm = [&](const JsonTypeDef &val, const std::string &path) {
        if (val.is_string()) {
            FlattenedTerm term;
            term.term_ = path + ":s:" + val.get<std::string>();
            terms.push_back(std::move(term));
        } else if (val.is_number_integer()) {
            FlattenedTerm term;
            term.term_ = path + ":i:" + EncodeInteger(val.get<int64_t>());
            terms.push_back(std::move(term));
        } else if (val.is_number_unsigned()) {
            FlattenedTerm term;
            term.term_ = path + ":i:" + EncodeInteger(static_cast<int64_t>(val.get<uint64_t>()));
            terms.push_back(std::move(term));
        } else if (val.is_number_float()) {
            FlattenedTerm term;
            term.term_ = path + ":d:" + EncodeDouble(val.get<double>());
            terms.push_back(std::move(term));
        } else if (val.is_boolean()) {
            FlattenedTerm term;
            term.term_ = path + ":b:" + (val.get<bool>() ? "1" : "0");
            terms.push_back(std::move(term));
        } else if (val.is_null()) {
            FlattenedTerm term;
            term.term_ = path + ":n:";
            terms.push_back(std::move(term));
        }
    };

    if (json_value.is_object()) {
        for (auto &item : json_value.items()) {
            std::string child_path = current_path.empty() ? std::string("$") + "." + item.key() : current_path + "." + item.key();
            FlattenRecursive(item.value(), child_path, terms);
        }
    } else if (json_value.is_array()) {
        size_t idx = 0;
        for (const auto &val : json_value) {
            std::string child_path = current_path + "[" + std::to_string(idx) + "]";
            FlattenRecursive(val, child_path, terms);
            ++idx;
        }
        // Emit parent array terms (without index) for 2-arg json_contains support
        // For root array (current_path is empty), use "$" as parent path
        // For nested arrays, use current_path as parent path
        std::string parent_path = current_path.empty() ? "$" : current_path;
        for (const auto &val : json_value) {
            AddScalarTerm(val, parent_path);
        }
    } else {
        AddScalarTerm(json_value, current_path);
        // Emit path exists term for json_exists_path support (for non-null scalar values)
        if (!json_value.is_null()) {
            FlattenedTerm path_term;
            path_term.term_ = current_path + ":p:";
            terms.push_back(std::move(path_term));
        }
    }
}

std::vector<FlattenedTerm> JsonFlattener::FlattenDocument(const uint8_t *bson_data, size_t bson_length) {
    std::vector<FlattenedTerm> terms;
    auto json_obj = JsonManager::from_bson(bson_data, bson_length);
    if (json_obj) {
        FlattenRecursive(*json_obj, "", terms);
    }
    return terms;
}

std::vector<FlattenedTerm> JsonFlattener::FlattenJson(const JsonTypeDef &json_obj) {
    std::vector<FlattenedTerm> terms;
    FlattenRecursive(json_obj, "", terms);
    return terms;
}

} // namespace infinity

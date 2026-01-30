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

module infinity_core:like.impl;

import :like;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

// Thread-local cache for compiled LIKE patterns
struct LikePatternCache {
    static constexpr size_t MAX_CACHE_SIZE = 256;

    std::unordered_map<std::string, re2::RE2> cache;
    std::vector<std::string> lru_list;

    re2::RE2 *GetOrCreate(const std::string &like_pattern, char escape_char) {
        // Create cache key: pattern + escape char as hex
        char escape_buf[8];
        snprintf(escape_buf, sizeof(escape_buf), "\\x%02x", static_cast<unsigned char>(escape_char));
        std::string cache_key = like_pattern;
        cache_key += escape_buf;

        auto it = cache.find(cache_key);
        if (it != cache.end()) {
            return &it->second;
        }

        // Cache miss - create new regex pattern
        if (cache.size() >= MAX_CACHE_SIZE) {
            if (!lru_list.empty()) {
                cache.erase(lru_list.front());
                lru_list.erase(lru_list.begin());
            }
        }

        // Convert LIKE to regex pattern
        std::string regex_pattern;
        regex_pattern.reserve(like_pattern.size() * 2);

        for (size_t i = 0; i < like_pattern.size(); ++i) {
            char c = like_pattern[i];
            if (c == escape_char && i + 1 < like_pattern.size()) {
                char next = like_pattern[++i];
                regex_pattern += re2::RE2::QuoteMeta(std::string(1, next));
            } else if (c == '%') {
                regex_pattern += ".*";
            } else if (c == '_') {
                regex_pattern += ".";
            } else {
                regex_pattern += re2::RE2::QuoteMeta(std::string(1, c));
            }
        }

        auto [inserted_it, success] = cache.try_emplace(cache_key, regex_pattern);
        if (success && inserted_it->second.ok()) {
            lru_list.push_back(cache_key);
            return &inserted_it->second;
        }

        return nullptr;
    }
};

// Thread-local cache instance
static thread_local LikePatternCache thread_cache;

bool LikeOperator(const char *left_ptr, size_t left_len, const char *right_ptr, size_t right_len, char escape_char, bool &pattern_error) {
    if (right_len == 0) {
        return left_len == 0;
    }
    if (right_len == 1 && right_ptr[0] == '%') {
        return true;
    }

    std::string pattern_str(right_ptr, right_len);
    re2::RE2 *cached_pattern = thread_cache.GetOrCreate(pattern_str, escape_char);
    if (!cached_pattern) {
        pattern_error = true;
        return false;
    }

    std::string input(left_ptr, left_len);
    return re2::RE2::FullMatch(input, *cached_pattern);
}

template <bool like>
struct LikeFunctionBase {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &right, TC &escape, TD &result) {
        const char *left_str;
        size_t left_len;
        const char *right_str;
        size_t right_len;
        const char *escape_str;
        size_t escape_len;
        GetReaderValue(left, left_str, left_len);
        GetReaderValue(right, right_str, right_len);
        GetReaderValue(escape, escape_str, escape_len);

        bool pattern_error = false;
        // ESCAPE clause is always exactly 1 character
        bool match = LikeOperator(left_str, left_len, right_str, right_len, escape_str[0], pattern_error);

        if (pattern_error) {
            Status status = Status::SyntaxError("Invalid LIKE pattern");
            RecoverableError(status);
        }

        if constexpr (like) {
            result.SetValue(!match);
        } else {
            result.SetValue(match);
        }
    }
};

using LikeFunction = LikeFunctionBase<false>;
using NotLikeFunction = LikeFunctionBase<true>;

void RegisterLikeFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "like";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_like_function(func_name,
                                         {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                         DataType(LogicalType::kBoolean),
                                         &ScalarFunction::TernaryFunction<VarcharT, VarcharT, VarcharT, BooleanT, LikeFunction>);
    function_set_ptr->AddFunction(varchar_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

void RegisterNotLikeFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "not_like";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_not_like_function(func_name,
                                             {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                             DataType(LogicalType::kBoolean),
                                             &ScalarFunction::TernaryFunction<VarcharT, VarcharT, VarcharT, BooleanT, NotLikeFunction>);
    function_set_ptr->AddFunction(varchar_not_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
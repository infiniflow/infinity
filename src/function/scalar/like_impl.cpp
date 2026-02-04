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
import third_party;

namespace infinity {

// Convert LIKE pattern to RE2 regex pattern
std::string LikeToRegexPattern(const std::string &like_pattern, char escape_char) {
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

    return regex_pattern;
}

// Thread-safe global cache for compiled LIKE patterns
struct LikePatternCache {
    static constexpr size_t MAX_CACHE_SIZE = 256;

    using CacheIter = std::unordered_map<std::string, std::shared_ptr<re2::RE2>>::iterator;
    std::unordered_map<std::string, std::shared_ptr<re2::RE2>> cache;
    std::list<CacheIter> lru_list;
    std::mutex mutex;

    std::shared_ptr<re2::RE2> GetOrCreate(const std::string &like_pattern, char escape_char) {
        // Create cache key: pattern + escape char as hex
        char escape_buf[8];
        snprintf(escape_buf, sizeof(escape_buf), "\\x%02x", static_cast<unsigned char>(escape_char));
        std::string cache_key = like_pattern;
        cache_key += escape_buf;

        std::lock_guard<std::mutex> lock(mutex);

        auto it = cache.find(cache_key);
        if (it != cache.end()) {
            for (auto lru_it = lru_list.begin(); lru_it != lru_list.end(); ++lru_it) {
                if (*lru_it == it) {
                    lru_list.erase(lru_it);
                    break;
                }
            }
            lru_list.push_back(it);
            return it->second;
        }

        if (cache.size() >= MAX_CACHE_SIZE && !lru_list.empty()) {
            // Remove least recently used entry
            auto oldest = lru_list.front();
            cache.erase(oldest);
            lru_list.pop_front();
        }

        std::string regex_pattern = LikeToRegexPattern(like_pattern, escape_char);
        auto regex = std::make_shared<re2::RE2>(regex_pattern);
        if (!regex->ok()) {
            return nullptr;
        }

        auto [inserted_it, success] = cache.try_emplace(cache_key, regex);
        if (success) {
            lru_list.push_back(inserted_it);
            return inserted_it->second;
        }

        return nullptr;
    }
};

// Global cache instance (shared across all threads)
static LikePatternCache global_cache;

bool LikeOperator(const char *left_ptr, size_t left_len, const char *right_ptr, size_t right_len, char escape_char, bool &pattern_error) {
    if (right_len == 0) {
        return left_len == 0;
    }
    if (right_len == 1 && right_ptr[0] == '%') {
        return true;
    }

    std::string pattern_str(right_ptr, right_len);
    auto cached_pattern = global_cache.GetOrCreate(pattern_str, escape_char);
    if (!cached_pattern) {
        pattern_error = true;
        return false;
    }

    std::string input(left_ptr, left_len);
    return re2::RE2::FullMatch(input, *cached_pattern);
}

// Batch processing for LIKE/NOT LIKE operator (Flat + Constant + Constant case)
// Processes all rows in a column vector at once for better performance.
void LikeOperatorBatch(auto &left,
                       const char *pattern_str,
                       size_t pattern_len,
                       char escape_char,
                       BooleanColumnWriter &result,
                       size_t count,
                       bool like,
                       bool &pattern_error) {
    // Fast path: empty pattern
    if (pattern_len == 0) {
        for (size_t i = 0; i < count; ++i) {
            const char *input_str;
            size_t input_len;
            GetReaderValue(left[i], input_str, input_len);
            bool match = (input_len == 0);
            result[i].SetValue(like ? match : !match);
        }
        return;
    }

    // Fast path: single '%' matches everything
    if (pattern_len == 1 && pattern_str[0] == '%') {
        for (size_t i = 0; i < count; ++i) {
            result[i].SetValue(like ? true : false);
        }
        return;
    }

    // Get or create cached RE2 pattern
    std::string pattern_str_obj(pattern_str, pattern_len);
    auto cached_pattern = global_cache.GetOrCreate(pattern_str_obj, escape_char);

    if (!cached_pattern) {
        pattern_error = true;
        for (size_t i = 0; i < count; ++i) {
            result[i].SetValue(like ? false : true);
        }
        return;
    }

    // Process all rows with RE2 and write directly to result
    for (size_t i = 0; i < count; ++i) {
        const char *input_str;
        size_t input_len;
        GetReaderValue(left[i], input_str, input_len);

        std::string input(input_str, input_len);
        bool match = re2::RE2::FullMatch(input, *cached_pattern);
        result[i].SetValue(like ? match : !match);
    }
}

template <bool like>
struct LikeFunctionBase {
    // Row-by-row execution: Process single row
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
            std::string pattern_msg(right_str, right_len);
            Status status = Status::SyntaxError(fmt::format("Invalid LIKE pattern: {}", pattern_msg));
            RecoverableError(status);
        }
        result.SetValue(like ? match : !match);
    }

    // Batch execution: Process all rows at once (Flat + Constant + Constant case)
    template <typename TA, typename TB, typename TC>
    static inline void RunBatch(TA &left, const TB &right, const TC &escape, BooleanColumnWriter &result, size_t count) {
        const char *pattern_str;
        size_t pattern_len;
        const char *escape_str;
        size_t escape_len;
        GetReaderValue(right, pattern_str, pattern_len);
        GetReaderValue(escape, escape_str, escape_len);

        bool pattern_error = false;
        LikeOperatorBatch(left, pattern_str, pattern_len, escape_str[0], result, count, like, pattern_error);

        if (pattern_error) {
            std::string pattern_msg(pattern_str, pattern_len);
            Status status = Status::SyntaxError(fmt::format("Invalid LIKE pattern: {}", pattern_msg));
            RecoverableError(status);
        }
    }
};

using LikeFunction = LikeFunctionBase<true>;
using NotLikeFunction = LikeFunctionBase<false>;

void RegisterLikeFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "like";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_like_function(func_name,
                                         {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                         DataType(LogicalType::kBoolean),
                                         &ScalarFunction::TernaryFunctionWithBatch<VarcharT, VarcharT, VarcharT, BooleanT, LikeFunction>);
    function_set_ptr->AddFunction(varchar_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

void RegisterNotLikeFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "not_like";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_not_like_function(func_name,
                                             {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                             DataType(LogicalType::kBoolean),
                                             &ScalarFunction::TernaryFunctionWithBatch<VarcharT, VarcharT, VarcharT, BooleanT, NotLikeFunction>);
    function_set_ptr->AddFunction(varchar_not_like_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
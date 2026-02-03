module infinity_core:regex.impl;

import :regex;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :column_vector;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

// Thread-safe global cache for compiled regex patterns
struct RegexPatternCache {
    static constexpr size_t MAX_CACHE_SIZE = 256;

    using CacheIter = std::unordered_map<std::string, std::shared_ptr<re2::RE2>>::iterator;
    std::unordered_map<std::string, std::shared_ptr<re2::RE2>> cache;
    std::list<CacheIter> lru_list;
    std::mutex mutex;

    std::shared_ptr<re2::RE2> GetOrCreate(const std::string &pattern_str) {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = cache.find(pattern_str);
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
            auto oldest = lru_list.front();
            cache.erase(oldest);
            lru_list.pop_front();
        }

        auto regex = std::make_shared<re2::RE2>(pattern_str);
        if (!regex->ok()) {
            return nullptr;
        }

        auto [inserted_it, success] = cache.try_emplace(pattern_str, regex);
        if (success) {
            lru_list.push_back(inserted_it);
            return inserted_it->second;
        }

        return nullptr;
    }
};

// Global cache instance (shared across all threads)
static RegexPatternCache global_cache;

// Batch processing for REGEX operator (Flat + Constant case)
// Processes all rows in a column vector at once for better performance.
void RegexOperatorBatch(auto &left, const char *pattern_str, size_t pattern_len, BooleanColumnWriter &result, size_t count, bool &pattern_error) {
    std::string pattern_str_obj(pattern_str, pattern_len);
    auto cached_regex = global_cache.GetOrCreate(pattern_str_obj);

    if (!cached_regex) {
        pattern_error = true;
        for (size_t i = 0; i < count; ++i) {
            result[i].SetValue(false);
        }
        return;
    }

    // Process all rows with RE2 and write directly to result
    for (size_t i = 0; i < count; ++i) {
        const char *input_str;
        size_t input_len;
        GetReaderValue(left[i], input_str, input_len);

        std::string input(input_str, input_len);
        bool match = re2::RE2::PartialMatch(input, *cached_regex);
        result[i].SetValue(match);
    }
}

struct RegexFunction {
    // Row-by-row execution: Process single row (for Flat + Flat case where pattern varies)
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &left, TB &right, TC &result) {
        const char *origin_str;
        size_t origin_len;
        const char *pattern_str;
        size_t pattern_len;
        GetReaderValue(left, origin_str, origin_len);
        GetReaderValue(right, pattern_str, pattern_len);
        std::string pattern_str_(pattern_str, pattern_len);

        // Try to get from cache
        auto cached_pattern = global_cache.GetOrCreate(pattern_str_);
        if (!cached_pattern) {
            Status status = Status::SyntaxError(fmt::format("Invalid regex pattern: {}", pattern_str_));
            RecoverableError(status);
        }
        std::string origin_str_(origin_str, origin_len);
        bool match = re2::RE2::PartialMatch(origin_str_, *cached_pattern);
        result.SetValue(match);
    }

    // Batch execution: Process all rows at once (Flat + Constant case)
    template <typename TA, typename TB>
    static inline void RunBatch(TA &left, const TB &right, BooleanColumnWriter &result, size_t count) {
        const char *pattern_str;
        size_t pattern_len;
        GetReaderValue(right, pattern_str, pattern_len);

        bool pattern_error = false;
        RegexOperatorBatch(left, pattern_str, pattern_len, result, count, pattern_error);

        if (pattern_error) {
            std::string pattern_msg(pattern_str, pattern_len);
            Status status = Status::SyntaxError(fmt::format("Invalid regex pattern: {}", pattern_msg));
            RecoverableError(status);
        }
    }
};

void RegisterRegexFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "regex";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction regex_function(func_name,
                                  {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                  DataType(LogicalType::kBoolean),
                                  &ScalarFunction::BinaryFunctionWithBatch<VarcharT, VarcharT, BooleanT, RegexFunction>);
    function_set_ptr->AddFunction(regex_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
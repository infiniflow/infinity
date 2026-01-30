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

// Thread-local cache for compiled regex patterns
struct RegexPatternCache {
    // Max cached patterns per thread
    static constexpr size_t MAX_CACHE_SIZE = 256;

    std::unordered_map<std::string, re2::RE2> cache;
    std::vector<std::string> lru_list;

    re2::RE2 *GetOrCreate(const std::string &pattern_str) {
        auto it = cache.find(pattern_str);
        if (it != cache.end()) {
            return &it->second;
        }

        // Cache miss - create new pattern
        if (cache.size() >= MAX_CACHE_SIZE) {
            // Simple LRU: remove oldest entry
            if (!lru_list.empty()) {
                cache.erase(lru_list.front());
                lru_list.erase(lru_list.begin());
            }
        }

        auto [inserted_it, success] = cache.try_emplace(pattern_str, pattern_str);
        if (success && inserted_it->second.ok()) {
            lru_list.push_back(pattern_str);
            return &inserted_it->second;
        }

        return nullptr;
    }
};

// Thread-local cache instance
static thread_local RegexPatternCache thread_cache;

struct RegexFunction {
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
        re2::RE2 *cached_pattern = thread_cache.GetOrCreate(pattern_str_);
        if (!cached_pattern) {
            Status status = Status::SyntaxError("Invalid regex pattern");
            RecoverableError(status);
        }
        std::string origin_str_(origin_str, origin_len);
        bool match = re2::RE2::PartialMatch(origin_str_, *cached_pattern);
        result.SetValue(match);
    }
};

void RegisterRegexFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "regex";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction regex_function(func_name,
                                  {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                  DataType(LogicalType::kBoolean),
                                  &ScalarFunction::BinaryFunction<VarcharT, VarcharT, BooleanT, RegexFunction>);
    function_set_ptr->AddFunction(regex_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
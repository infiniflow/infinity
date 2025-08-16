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

struct RegexFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &left, TB &right, TC &result) {
        const char *origin_str;
        size_t origin_len;
        const char *pattern_str;
        size_t pattern_len;
        GetReaderValue(left, origin_str, origin_len);
        GetReaderValue(right, pattern_str, pattern_len);
        std::string origin_str_(origin_str, origin_len);
        std::string pattern_str_(pattern_str, pattern_len);
        bool match = re2::RE2::PartialMatch(origin_str_, pattern_str_);
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
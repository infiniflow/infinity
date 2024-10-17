module;

#include <re2/re2.h>

module regex;

import stl;
import catalog;
import status;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import logical_type;
import internal_types;
import data_type;
import logger;
import column_vector;

namespace infinity {

struct RegexFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &left, TB &right, TC &result) {
        const char * origin_str;
        SizeT origin_len;
        const char * pattern_str;
        SizeT pattern_len;
        GetReaderValue(left, origin_str, origin_len);
        GetReaderValue(right, pattern_str, pattern_len);
        String origin_str_(origin_str, origin_len);
        String pattern_str_(pattern_str, pattern_len);
        bool match = re2::RE2::PartialMatch(origin_str_, pattern_str_);
        result.SetValue(match);
    }
};


void RegisterRegexFunction(const UniquePtr<Catalog> &catalog_ptr){
    String func_name = "regex";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction regex_function(func_name,
                                     {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                     DataType(LogicalType::kBoolean),
                                     &ScalarFunction::BinaryFunction<VarcharT, VarcharT, BooleanT, RegexFunction>);
    function_set_ptr->AddFunction(regex_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
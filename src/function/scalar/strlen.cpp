module;

#include <string>

module strlen;

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

struct StrlenFunction {
    template <typename TA, typename TR>
    static inline void Run(TA &input, TR &result) {
        const char *input_str;
        SizeT input_len;
        GetReaderValue(input, input_str, input_len);

        int length = static_cast<int>(input_len);

        result.SetValue(length);
    }
};

void RegisterStrlenFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "strlen";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction strlen_function(func_name,
                                   {DataType(LogicalType::kVarchar)},
                                   DataType(LogicalType::kInteger),
                                   &ScalarFunction::UnaryFunction<VarcharT, IntegerT, StrlenFunction>);

    function_set_ptr->AddFunction(strlen_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

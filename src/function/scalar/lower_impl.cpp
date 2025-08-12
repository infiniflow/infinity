module;

module infinity_core:lower.impl;

import :lower;
import :stl;
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

struct LowerFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &result, TC left_ptr, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void LowerFunction::Run(VarcharT &left, VarcharT &result, ColumnVector *left_ptr, ColumnVector *result_ptr) {
    Span<const char> left_v = left_ptr->GetVarcharInner(left);
    const char *input = left_v.data();
    SizeT input_len = left_v.size();
    String lower_str(input, input_len);
    ToLower(lower_str);
    result_ptr->AppendVarcharInner(lower_str, result);
}

void RegisterLowerFunction(NewCatalog *catalog_ptr) {
    String func_name = "lower";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction lower_function(func_name,
                                  {DataType(LogicalType::kVarchar)},
                                  {DataType(LogicalType::kVarchar)},
                                  &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, VarcharT, LowerFunction>);
    function_set_ptr->AddFunction(lower_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
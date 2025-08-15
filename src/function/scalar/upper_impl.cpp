module;

module infinity_core:upper.impl;

import :upper;
import :column_vector;
import :stl;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

struct UpperFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &result, TC left_ptr, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void UpperFunction::Run(VarcharT &left, VarcharT &result, ColumnVector *left_ptr, ColumnVector *result_ptr) {
    std::span<const char> left_v = left_ptr->GetVarcharInner(left);
    const char *input = left_v.data();
    size_t input_len = left_v.size();
    std::string upper_str(input, input_len);
    ToUpper(upper_str);
    result_ptr->AppendVarcharInner(upper_str, result);
}

void RegisterUpperFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "upper";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction upper_function(func_name,
                                  {DataType(LogicalType::kVarchar)},
                                  {DataType(LogicalType::kVarchar)},
                                  &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, VarcharT, UpperFunction>);
    function_set_ptr->AddFunction(upper_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
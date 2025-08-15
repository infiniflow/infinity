module infinity_core:rtrim.impl;

import :rtrim;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :column_vector;

import std.compat;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

struct RtrimFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &result, TC left_ptr, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void RtrimFunction::Run(VarcharT &left, VarcharT &result, ColumnVector *left_ptr, ColumnVector *result_ptr) {
    const char *input = nullptr;
    size_t input_len = 0;
    std::span<const char> left_v = left_ptr->GetVarcharInner(left);
    input = left_v.data();
    input_len = left_v.size();
    long pos = input_len - 1;
    while (pos > -1 && std::isspace(static_cast<unsigned char>(input[pos]))) {
        pos--;
    }

    std::span<const char> res_span = std::span<const char>(input, pos + 1);
    result_ptr->AppendVarcharInner(res_span, result);
}

void RegisterRtrimFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "rtrim";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction rtrim_function(func_name,
                                  {DataType(LogicalType::kVarchar)},
                                  {DataType(LogicalType::kVarchar)},
                                  &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, VarcharT, RtrimFunction>);
    function_set_ptr->AddFunction(rtrim_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
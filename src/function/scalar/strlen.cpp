module;

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
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &result, TC left_ptr, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void StrlenFunction::Run(VarcharT &left, IntT &result, ColumnVector *left_ptr, ColumnVector *result_ptr) {
    Span<const char> left_v = left_ptr->GetVarcharInner(left);
    const char *input = left_v.data();
    SizeT input_len = left_v.size();
    IntT length = static_cast<IntT>(input_len);
    result_ptr->AppendIntInner(length, result);
}

void RegisterStrlenFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "strlen";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction strlen_function(func_name,
                                   {DataType(LogicalType::kVarchar)},
                                   {DataType(LogicalType::kInt)},
                                   &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, IntT, StrlenFunction>);

    function_set_ptr->AddFunction(strlen_function);
    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

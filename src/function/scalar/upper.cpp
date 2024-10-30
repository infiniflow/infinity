module;

module upper;

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

struct UpperFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &result, TC left_ptr, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void UpperFunction::Run(VarcharT &left, VarcharT &result, ColumnVector *left_ptr, ColumnVector *result_ptr) {
    Span<const char> left_v = left_ptr->GetVarcharInner(left);
    const char *input = left_v.data();
    SizeT input_len = left_v.size();
    String upper_str(input, input_len);
    ToUpper(upper_str);
    result_ptr->AppendVarcharInner(upper_str, result);
}


void RegisterUpperFunction(const UniquePtr<Catalog> &catalog_ptr){
    String func_name = "upper";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction upper_function(func_name,
                                     {DataType(LogicalType::kVarchar)},
                                     {DataType(LogicalType::kVarchar)},
                                     &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, VarcharT, UpperFunction>);
    function_set_ptr->AddFunction(upper_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
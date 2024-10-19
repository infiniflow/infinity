module;

#include<cctype>

module rtrim;

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
    SizeT input_len = 0;
    Span<const char> left_v = left_ptr->GetVarcharInner(left);
    input = left_v.data();
    input_len = left_v.size();
    long pos = input_len-1;
    while (pos > -1 && std::isspace(static_cast<unsigned char>(input[pos]))) {
        pos--;
    }

    Span<const char> res_span = Span<const char>(input, pos+1);
    result_ptr->AppendVarcharInner(res_span, result);
}


void RegisterRtrimFunction(const UniquePtr<Catalog> &catalog_ptr){
    String func_name = "rtrim";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction rtrim_function(func_name,
                                     {DataType(LogicalType::kVarchar)},
                                     {DataType(LogicalType::kVarchar)},
                                     &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, VarcharT, RtrimFunction>);
    function_set_ptr->AddFunction(rtrim_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
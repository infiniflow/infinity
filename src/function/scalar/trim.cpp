module;

#include<cctype>

module trim;

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

struct TrimFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &result, TC left_ptr, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void TrimFunction::Run(VarcharT &left, VarcharT &result, ColumnVector *left_ptr, ColumnVector *result_ptr) {
    const char *input = nullptr;
    SizeT input_len = 0;
    Span<const char> left_v = left_ptr->GetVarcharInner(left);
    input = left_v.data();
    input_len = left_v.size();
    SizeT lpos = 0;
    while (lpos < input_len && std::isspace(static_cast<unsigned char>(input[lpos]))) {
        lpos++;
    }

    if (lpos == input_len) {
        // Construct empty varchar value;
        Span<const char> substr_span = Span<const char>(input, 0);
        result_ptr->AppendVarcharInner(substr_span, result);
        return;
    }

    long rpos = input_len-1;
    while (rpos > -1 && std::isspace(static_cast<unsigned char>(input[rpos]))) {
        rpos--;
    }

    Span<const char> res_span = Span<const char>(&input[lpos], rpos-lpos+1);
    result_ptr->AppendVarcharInner(res_span, result);
}


void RegisterTrimFunction(const UniquePtr<Catalog> &catalog_ptr){
    String func_name = "trim";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction trim_function(func_name,
                                     {DataType(LogicalType::kVarchar)},
                                     {DataType(LogicalType::kVarchar)},
                                     &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, VarcharT, TrimFunction>);
    function_set_ptr->AddFunction(trim_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
module;

module md5;

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

struct Md5Function {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA &left, TB &result, TC left_ptr, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void Md5Function::Run(VarcharT &left, VarcharT &result, ColumnVector *left_ptr, ColumnVector *result_ptr) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    const char *input = nullptr;
    SizeT input_len = 0;
    Span<const char> left_v = left_ptr->GetVarcharInner(left);
    input = left_v.data();
    input_len = left_v.size();
    MD5(reinterpret_cast<const unsigned char *>(input), input_len, digest);
    Span<const char> digest_span = Span<const char>(reinterpret_cast<const char *>(digest), MD5_DIGEST_LENGTH);
    result_ptr->AppendVarcharInner(digest_span, result);
}


void RegisterMd5Function(const UniquePtr<Catalog> &catalog_ptr){
    String func_name = "md5";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction md5_function(func_name,
                                     {DataType(LogicalType::kVarchar)},
                                     {DataType(LogicalType::kVarchar)},
                                     &ScalarFunction::UnaryFunctionVarlenToVarlen<VarcharT, VarcharT, Md5Function>);
    function_set_ptr->AddFunction(md5_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
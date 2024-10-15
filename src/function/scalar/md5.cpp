module;

#include <openssl/md5.h>

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
    template <typename TA, typename TB>
    static inline void Run(TA &left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void Md5Function::Run(VarcharT &left, VarcharT &result) {
    unsigned char digest[MD5_DIGEST_LENGTH];
    const char *input = nullptr;
    SizeT input_len = 0;
    //GetReaderValue(left, input, input_len);
    MD5(reinterpret_cast<const unsigned char *>(input), input_len, digest);

}


struct ColumnValueReaderMd5Function {
    template <typename TA, typename TB>
    static inline void Run(TA &left, TB &result) {
        unsigned char digest[MD5_DIGEST_LENGTH];
        const char *input = nullptr;
        SizeT input_len = 0;
        GetReaderValue(left, input, input_len);
        MD5(reinterpret_cast<const unsigned char *>(input), input_len, digest);
        SetReaderValue(result, reinterpret_cast<const char*>(digest), MD5_DIGEST_LENGTH);
    }
};


void RegisterMd5Function(const UniquePtr<Catalog> &catalog_ptr){
    String func_name = "md5";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction md5_function(func_name,
                                     {DataType(LogicalType::kVarchar)},
                                     {DataType(LogicalType::kVarchar)},
                                     &ScalarFunction::UnaryFunction<VarcharT, VarcharT, ColumnValueReaderMd5Function>);
    function_set_ptr->AddFunction(md5_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
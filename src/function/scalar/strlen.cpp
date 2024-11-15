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

namespace infinity {

struct StrlenFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA &input, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }
};

template <>
inline bool StrlenFunction::Run(VarcharT &input, int &result) {
    if (input.IsInlined()) {
        result = input.length_;
    } else {
        const char *data = input.vector_.prefix_;
        SizeT actual_length = strlen(data);
        result = static_cast<int32_t>(actual_length);
    }
    return true;
}

void RegisterStrlenFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "strlen";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction strlen_function(func_name,
                                   {DataType(LogicalType::kVarchar)},
                                   {DataType(LogicalType::kInteger)},
                                   &ScalarFunction::UnaryFunctionWithFailure<VarcharT, IntegerT, StrlenFunction>);

    function_set_ptr->AddFunction(strlen_function);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

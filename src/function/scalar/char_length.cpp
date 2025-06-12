module;

module char_length;

import stl;
import new_catalog;
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

struct CharLengthFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA &left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        return false;
    }
};

template <>
inline bool CharLengthFunction::Run(VarcharT &left, int &result) {
    result = left.length_;
    return true;
}

void RegisterCharLengthFunction(NewCatalog *catalog_ptr) {
    String func_name = "char_length";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction char_length_function(func_name,
                                        {DataType(LogicalType::kVarchar)},
                                        {DataType(LogicalType::kInteger)},
                                        &ScalarFunction::UnaryFunctionWithFailure<VarcharT, IntegerT, CharLengthFunction>);
    function_set_ptr->AddFunction(char_length_function);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
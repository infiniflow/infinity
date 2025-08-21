module infinity_core:position.impl;

import :position;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;
import :column_vector;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

struct PositionFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &first, TB &second, TC &result, ColumnVector *first_ptr, ColumnVector *second_ptr, ColumnVector *) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void
PositionFunction::Run(VarcharT &first, VarcharT &second, IntegerT &result, ColumnVector *first_ptr, ColumnVector *second_ptr, ColumnVector *) {
    std::span<const char> first_v = first_ptr->GetVarcharInner(first);
    std::span<const char> second_v = second_ptr->GetVarcharInner(second);
    std::string first_str(first_v.data(), first_v.size());
    std::string second_str(second_v.data(), second_v.size());
    std::string::size_type pos = first_str.find(second_str);
    if (pos == std::string::npos) {
        result = 0;
    } else {
        result = pos + 1;
    }
}

void RegisterPositionFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "char_position";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_pos_int32(func_name,
                                     {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                     {DataType(LogicalType::kInteger)},
                                     &ScalarFunction::BinaryFunctionVarlenToVarlen<VarcharT, VarcharT, IntegerT, PositionFunction>);
    function_set_ptr->AddFunction(varchar_pos_int32);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
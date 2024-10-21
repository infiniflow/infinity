module;

module position;

import stl;
import catalog;
import status;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import column_vector;

import third_party;
import logical_type;
import internal_types;
import data_type;
import logger;

namespace infinity {

struct PositionFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA &first, TB &second, TC &result, ColumnVector *first_ptr, ColumnVector *second_ptr, ColumnVector *) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void PositionFunction::Run(VarcharT &first, VarcharT &second, IntegerT &result, ColumnVector *first_ptr, ColumnVector * second_ptr, ColumnVector *) {
    Span<const char> first_v = first_ptr->GetVarcharInner(first);
    Span<const char> second_v = second_ptr->GetVarcharInner(second);
    String first_str(first_v.data(), first_v.size());
    String second_str(second_v.data(), second_v.size());
    String::size_type pos = first_str.find(second_str);
    if(pos == String::npos){
        result = 0;
    }else{
        result = pos + 1;
    }
}



void RegisterPositionFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "str_position";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_pos_int32(func_name,
                                  {DataType(LogicalType::kVarchar), DataType(LogicalType::kVarchar)},
                                  {DataType(LogicalType::kInteger)},
                                  &ScalarFunction::BinaryFunctionVarlenToVarlen<VarcharT, VarcharT, IntegerT, PositionFunction>);
    function_set_ptr->AddFunction(varchar_pos_int32);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
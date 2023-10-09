//
// Created by jinhai on 23-1-10.
//

#include "plus.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct PlusFunction {
    template <typename TA, typename TB>
    static inline bool Run(TA value, TB &result) {
        NotImplementError("Not implemented plus function")
    }
};

template <>
inline bool PlusFunction::Run(MixedT value, MixedT &result) {
    if (value.type == MixedValueType::kInteger || value.type == MixedValueType::kFloat) {
        result = value;
        return true;
    } else {
        result.type = MixedValueType::kNull;
        return false;
    }
}

void RegisterPlusFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "+";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    // Plus operator
    ScalarFunction plus_int8(func_name, {DataType(LogicalType::kTinyInt)}, DataType(LogicalType::kTinyInt), &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int8);

    ScalarFunction plus_int16(func_name, {DataType(LogicalType::kSmallInt)}, DataType(LogicalType::kSmallInt), &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int16);

    ScalarFunction plus_int32(func_name, {DataType(LogicalType::kInteger)}, DataType(LogicalType::kInteger), &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int32);

    ScalarFunction plus_int64(func_name, {DataType(LogicalType::kBigInt)}, DataType(LogicalType::kBigInt), &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int64);

    ScalarFunction plus_int128(func_name, {DataType(LogicalType::kHugeInt)}, DataType(LogicalType::kHugeInt), &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_int128);

    ScalarFunction plus_float(func_name, {DataType(LogicalType::kFloat)}, DataType(LogicalType::kFloat), &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_float);

    ScalarFunction plus_double(func_name, {DataType(LogicalType::kDouble)}, DataType(LogicalType::kDouble), &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_double);

    ScalarFunction plus_decimal(func_name, {DataType(LogicalType::kDecimal)}, DataType(LogicalType::kDecimal), &ScalarFunction::NoOpFunction);
    function_set_ptr->AddFunction(plus_decimal);

    ScalarFunction plus_mixed(func_name,
                              {DataType(LogicalType::kMixed)},
                              DataType(LogicalType::kMixed),
                              &ScalarFunction::UnaryFunctionWithFailure<MixedT, MixedT, PlusFunction>);
    function_set_ptr->AddFunction(plus_mixed);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

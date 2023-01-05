//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct AddFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left + right;
    }
};

void
RegisterAddFunction(const UniquePtr<Catalog> &catalog_ptr) {
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>("+");

//    GenerateAddFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    ScalarFunction add_function_int8(
            "+",
            { DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt) },
            { DataType(LogicalType::kSmallInt) },
            &ScalarFunction::BinaryFunction<TinyIntT, TinyIntT , SmallIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int8);

    ScalarFunction add_function_int16(
            "+",
            { DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt) },
            { DataType(LogicalType::kInteger) },
            &ScalarFunction::BinaryFunction<SmallIntT, SmallIntT, IntegerT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int16);

    ScalarFunction add_function_int32(
            "+",
            { DataType(LogicalType::kInteger), DataType(LogicalType::kInteger) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunction<IntegerT, IntegerT, BigIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int32);

    ScalarFunction add_function_int64(
            "+",
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunction<BigIntT, BigIntT, BigIntT, AddFunction>);
    function_set_ptr->AddFunction(add_function_int64);

    ScalarFunction add_function_float(
            "+",
            { DataType(LogicalType::kFloat), DataType(LogicalType::kFloat) },
            { DataType(LogicalType::kFloat) },
            &ScalarFunction::BinaryFunction<FloatT, FloatT, FloatT, AddFunction>);
    function_set_ptr->AddFunction(add_function_float);

    ScalarFunction add_function_double(
            "+",
            { DataType(LogicalType::kDouble), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunction<DoubleT, DoubleT, DoubleT, AddFunction>);
    function_set_ptr->AddFunction(add_function_double);

    // TODO: other type add need to be given.
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
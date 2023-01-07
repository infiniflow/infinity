//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct SubFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left - right;
    }
};

void
RegisterSubFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("-");

//    GenerateAddFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    ScalarFunction sub_function_int8(
            "-",
            { DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt) },
            { DataType(LogicalType::kTinyInt) },
            &ScalarFunction::BinaryFunction<TinyIntT, TinyIntT, TinyIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int8);

    ScalarFunction sub_function_int16(
            "-",
            { DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt) },
            { DataType(LogicalType::kSmallInt) },
            &ScalarFunction::BinaryFunction<SmallIntT, SmallIntT, SmallIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int16);

    ScalarFunction sub_function_int32(
            "-",
            { DataType(LogicalType::kInteger), DataType(LogicalType::kInteger) },
            { DataType(LogicalType::kInteger) },
            &ScalarFunction::BinaryFunction<IntegerT, IntegerT, IntegerT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int32);

    ScalarFunction sub_function_int64(
            "-",
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunction<BigIntT, BigIntT, BigIntT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_int64);

    ScalarFunction sub_function_float(
            "-",
            { DataType(LogicalType::kFloat), DataType(LogicalType::kFloat) },
            { DataType(LogicalType::kFloat) },
            &ScalarFunction::BinaryFunction<FloatT, FloatT, FloatT, SubFunction>);
    function_set_ptr->AddFunction(sub_function_float);

    ScalarFunction sub_function_double(
            "-",
            { DataType(LogicalType::kDouble), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunction<double, double, double, SubFunction>);
    function_set_ptr->AddFunction(sub_function_double);

    // TODO: other type sub need to be given.

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
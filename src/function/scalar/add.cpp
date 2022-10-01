//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include <cmath>

namespace infinity {

struct AddFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left + right;
    }
};

void
RegisterAddFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("+");

//    GenerateAddFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    ScalarFunction add_function_int8(
            "+",
            { LogicalType(LogicalTypeId::kTinyInt), LogicalType(LogicalTypeId::kTinyInt) },
            { LogicalType(LogicalTypeId::kSmallInt) },
            &ScalarFunction::BinaryFunction<int8_t , int8_t , int16_t, AddFunction>);
    function_set_ptr->AddFunction(add_function_int8);

    ScalarFunction add_function_int16(
            "+",
            { LogicalType(LogicalTypeId::kSmallInt), LogicalType(LogicalTypeId::kSmallInt) },
            { LogicalType(LogicalTypeId::kInteger) },
            &ScalarFunction::BinaryFunction<int16_t , int16_t , int32_t, AddFunction>);
    function_set_ptr->AddFunction(add_function_int16);

    ScalarFunction add_function_int32(
            "+",
            { LogicalType(LogicalTypeId::kInteger), LogicalType(LogicalTypeId::kInteger) },
            { LogicalType(LogicalTypeId::kBigInt) },
            &ScalarFunction::BinaryFunction<int32_t , int32_t , int64_t, AddFunction>);
    function_set_ptr->AddFunction(add_function_int32);

    ScalarFunction add_function_int64(
            "+",
            { LogicalType(LogicalTypeId::kBigInt), LogicalType(LogicalTypeId::kBigInt) },
            { LogicalType(LogicalTypeId::kBigInt) },
            &ScalarFunction::BinaryFunction<int64_t , int64_t , int64_t, AddFunction>);
    function_set_ptr->AddFunction(add_function_int64);

    ScalarFunction add_function_float(
            "+",
            { LogicalType(LogicalTypeId::kFloat), LogicalType(LogicalTypeId::kFloat) },
            { LogicalType(LogicalTypeId::kFloat) },
            &ScalarFunction::BinaryFunction<float , float , float, AddFunction>);
    function_set_ptr->AddFunction(add_function_float);

    ScalarFunction add_function_double(
            "+",
            { LogicalType(LogicalTypeId::kDouble), LogicalType(LogicalTypeId::kDouble) },
            { LogicalType(LogicalTypeId::kDouble) },
            &ScalarFunction::BinaryFunction<double , double , double, AddFunction>);
    function_set_ptr->AddFunction(add_function_double);

    // TODO: other type add need to be given.
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
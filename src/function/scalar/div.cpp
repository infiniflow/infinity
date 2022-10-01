//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include <cmath>

namespace infinity {

struct DivFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left / right;
    }
};

void
RegisterDivFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("/");

    ScalarFunction div_function_int8(
            "/",
            { LogicalType(LogicalTypeId::kTinyInt), LogicalType(LogicalTypeId::kTinyInt) },
            { LogicalType(LogicalTypeId::kTinyInt) },
            &ScalarFunction::BinaryFunction<int8_t, int8_t, int8_t, DivFunction>);
    function_set_ptr->AddFunction(div_function_int8);

    ScalarFunction div_function_int16(
            "/",
            { LogicalType(LogicalTypeId::kSmallInt), LogicalType(LogicalTypeId::kSmallInt) },
            { LogicalType(LogicalTypeId::kSmallInt) },
            &ScalarFunction::BinaryFunction<int16_t, int16_t, int16_t, DivFunction>);
    function_set_ptr->AddFunction(div_function_int16);

    ScalarFunction div_function_int32(
            "/",
            { LogicalType(LogicalTypeId::kInteger), LogicalType(LogicalTypeId::kInteger) },
            { LogicalType(LogicalTypeId::kInteger) },
            &ScalarFunction::BinaryFunction<int32_t, int32_t, int32_t, DivFunction>);
    function_set_ptr->AddFunction(div_function_int32);

    ScalarFunction div_function_int64(
            "/",
            { LogicalType(LogicalTypeId::kBigInt), LogicalType(LogicalTypeId::kBigInt) },
            { LogicalType(LogicalTypeId::kBigInt) },
            &ScalarFunction::BinaryFunction<int64_t, int64_t, int64_t, DivFunction>);
    function_set_ptr->AddFunction(div_function_int64);

    ScalarFunction div_function_float(
            "/",
            { LogicalType(LogicalTypeId::kFloat), LogicalType(LogicalTypeId::kFloat) },
            { LogicalType(LogicalTypeId::kFloat) },
            &ScalarFunction::BinaryFunction<float, float, float, DivFunction>);
    function_set_ptr->AddFunction(div_function_float);

    ScalarFunction div_function_double(
            "/",
            { LogicalType(LogicalTypeId::kDouble), LogicalType(LogicalTypeId::kDouble) },
            { LogicalType(LogicalTypeId::kDouble) },
            &ScalarFunction::BinaryFunction<double, double, double, DivFunction>);
    function_set_ptr->AddFunction(div_function_double);

    // TODO: other type divide need to be given.

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
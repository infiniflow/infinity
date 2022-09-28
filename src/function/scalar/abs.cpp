//
// Created by JinHai on 2022/9/14.
//

#include "abs.h"
#include "function/scalar_function.h"

namespace infinity {

struct AbsFunctionInt {
    template <typename TA, typename TB>
    static inline TA Execute(TB value) {
        if(value == std::numeric_limits<TB>::min()) {
            ExecutorError("Overflow on abs(" + std::to_string(value) + ").");
        }
        return value < 0 ? -value : value;
    }
};

struct AbsFunctionFloat {
    template <typename TA, typename TB>
    static inline TA Execute(TB value) {
        return value < 0 ? -value : value;
    }
};


void
RegisterAbsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("abs");

    ScalarFunction abs_int8(
            "abs",
            { LogicalType(LogicalTypeId::kTinyInt) },
            { LogicalType(LogicalTypeId::kTinyInt) },
            &ScalarFunction::UnaryFunction<int8_t, int8_t, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int8);

    ScalarFunction abs_int16(
            "abs",
            { LogicalType(LogicalTypeId::kSmallInt) },
            { LogicalType(LogicalTypeId::kSmallInt) },
            &ScalarFunction::UnaryFunction<int16_t, int16_t, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int16);

    ScalarFunction abs_int32(
            "abs",
            { LogicalType(LogicalTypeId::kInteger) },
            { LogicalType(LogicalTypeId::kInteger) },
            &ScalarFunction::UnaryFunction<int32_t, int32_t, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int32);

    ScalarFunction abs_int64(
            "abs",
            { LogicalType(LogicalTypeId::kBigInt) },
            { LogicalType(LogicalTypeId::kBigInt) },
            &ScalarFunction::UnaryFunction<int64_t, int64_t, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int64);

    ScalarFunction abs_float(
            "abs",
            { LogicalType(LogicalTypeId::kFloat) },
            { LogicalType(LogicalTypeId::kFloat) },
            &ScalarFunction::UnaryFunction<float, float, AbsFunctionFloat>);
    function_set_ptr->AddFunction(abs_float);

    ScalarFunction abs_double(
            "abs",
            { LogicalType(LogicalTypeId::kDouble) },
            { LogicalType(LogicalTypeId::kDouble) },
            &ScalarFunction::UnaryFunction<float, float, AbsFunctionFloat>);
    function_set_ptr->AddFunction(abs_double);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
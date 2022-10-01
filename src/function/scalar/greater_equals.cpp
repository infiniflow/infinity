//
// Created by JinHai on 2022/9/28.
//

#include "greater_equals.h"
#include "function/scalar_function.h"
#include <cmath>

namespace infinity {

struct GreaterEqualsFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left > right;
    }
};

template<typename DataType>
static void
GenerateGreaterEqualsFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, LogicalTypeId type_id) {

    ScalarFunction greater_equals_function(
            ">=",
            { LogicalType(type_id), LogicalType(type_id) },
            { LogicalType(LogicalTypeId::kBoolean) },
            &ScalarFunction::BinaryFunction<DataType, DataType, bool, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(greater_equals_function);
}

void
RegisterGreaterEqualsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(">=");

    GenerateGreaterEqualsFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    GenerateGreaterEqualsFunction<int8_t>(function_set_ptr, LogicalTypeId::kTinyInt);
    GenerateGreaterEqualsFunction<int16_t>(function_set_ptr, LogicalTypeId::kSmallInt);
    GenerateGreaterEqualsFunction<int32_t>(function_set_ptr, LogicalTypeId::kInteger);
    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kBigInt);
    GenerateGreaterEqualsFunction<float>(function_set_ptr, LogicalTypeId::kFloat);
    GenerateGreaterEqualsFunction<double>(function_set_ptr, LogicalTypeId::kDouble);

    // TODO: need to review following function
    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDate);
    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kInterval);
    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDateTime);
    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kTime);

    // FIXME: decimal isn't int64
    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDecimal);

    // TODO: need to review following function
    GenerateGreaterEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kVarchar);
    GenerateGreaterEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
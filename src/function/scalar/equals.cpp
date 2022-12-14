//
// Created by JinHai on 2022/9/28.
//

#include "equals.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct EqualsFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left == right;
    }
};

template<typename DataType>
static void
GenerateEqualsFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, LogicalTypeId type_id) {

    ScalarFunction equals_function(
            "=",
            { LogicalType(type_id), LogicalType(type_id) },
            { LogicalType(LogicalTypeId::kBoolean) },
            &ScalarFunction::BinaryFunction<DataType, DataType, bool, EqualsFunction>);
    function_set_ptr->AddFunction(equals_function);
}

void
RegisterEqualsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("=");

    GenerateEqualsFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    GenerateEqualsFunction<int8_t>(function_set_ptr, LogicalTypeId::kTinyInt);
    GenerateEqualsFunction<int16_t>(function_set_ptr, LogicalTypeId::kSmallInt);
    GenerateEqualsFunction<int32_t>(function_set_ptr, LogicalTypeId::kInteger);
    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kBigInt);
    GenerateEqualsFunction<float>(function_set_ptr, LogicalTypeId::kFloat);
    GenerateEqualsFunction<double>(function_set_ptr, LogicalTypeId::kDouble);

    // TODO: need to review following function
    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDate);
    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kInterval);
    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDateTime);
    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kTime);

    // FIXME: decimal isn't int64
    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDecimal);

    // TODO: need to review following function
    GenerateEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kVarchar);
    GenerateEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}

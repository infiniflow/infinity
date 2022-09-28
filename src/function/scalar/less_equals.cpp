//
// Created by JinHai on 2022/9/28.
//

#include "less_equals.h"
#include "function/scalar_function.h"
#include <cmath>

namespace infinity {

struct LessEqualsFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left > right;
    }
};

template<typename DataType>
static void
GenerateLessEqualsFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, LogicalTypeId type_id) {

    ScalarFunction less_equals_function(
            "<=",
            { LogicalType(type_id), LogicalType(type_id) },
            { LogicalType(LogicalTypeId::kBoolean) },
            &ScalarFunction::BinaryFunction<DataType, DataType, bool, LessEqualsFunction>);
    function_set_ptr->AddFunction(less_equals_function);
}

void
RegisterLessEqualsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("<=");

    GenerateLessEqualsFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    GenerateLessEqualsFunction<int8_t>(function_set_ptr, LogicalTypeId::kTinyInt);
    GenerateLessEqualsFunction<int16_t>(function_set_ptr, LogicalTypeId::kSmallInt);
    GenerateLessEqualsFunction<int32_t>(function_set_ptr, LogicalTypeId::kInteger);
    GenerateLessEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kBigInt);
    GenerateLessEqualsFunction<float>(function_set_ptr, LogicalTypeId::kFloat);
    GenerateLessEqualsFunction<double>(function_set_ptr, LogicalTypeId::kDouble);

    // TODO: need to review following function
    GenerateLessEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDate);
    GenerateLessEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kInterval);
    GenerateLessEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDateTime);
    GenerateLessEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kTime);

    // FIXME: decimal isn't int64
    GenerateLessEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDecimal);

    // TODO: need to review following function
    GenerateLessEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kVarchar);
    GenerateLessEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kText);
}

}
//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include <cmath>

namespace infinity {

struct LessFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left < right;
    }
};

template<typename DataType>
static void
GenerateLessFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, LogicalTypeId type_id) {

    ScalarFunction less_function(
            "<",
            { LogicalType(type_id), LogicalType(type_id) },
            { LogicalType(LogicalTypeId::kBoolean) },
            &ScalarFunction::BinaryFunction<DataType, DataType, bool, LessFunction>);
    function_set_ptr->AddFunction(less_function);
}

void
RegisterLessFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("<");

    GenerateLessFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    GenerateLessFunction<int8_t>(function_set_ptr, LogicalTypeId::kTinyInt);
    GenerateLessFunction<int16_t>(function_set_ptr, LogicalTypeId::kSmallInt);
    GenerateLessFunction<int32_t>(function_set_ptr, LogicalTypeId::kInteger);
    GenerateLessFunction<int64_t>(function_set_ptr, LogicalTypeId::kBigInt);
    GenerateLessFunction<float>(function_set_ptr, LogicalTypeId::kFloat);
    GenerateLessFunction<double>(function_set_ptr, LogicalTypeId::kDouble);

    // TODO: need to review following function
    GenerateLessFunction<int64_t>(function_set_ptr, LogicalTypeId::kDate);
    GenerateLessFunction<int64_t>(function_set_ptr, LogicalTypeId::kInterval);
    GenerateLessFunction<int64_t>(function_set_ptr, LogicalTypeId::kDateTime);
    GenerateLessFunction<int64_t>(function_set_ptr, LogicalTypeId::kTime);

    // FIXME: decimal isn't int64
    GenerateLessFunction<int64_t>(function_set_ptr, LogicalTypeId::kDecimal);

    // TODO: need to review following function
    GenerateLessFunction<std::string>(function_set_ptr, LogicalTypeId::kVarchar);
    GenerateLessFunction<std::string>(function_set_ptr, LogicalTypeId::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
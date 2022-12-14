//
// Created by JinHai on 2022/9/28.
//

#include "greater.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct GreaterFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left > right;
    }
};

template<typename DataType>
static void
GenerateGreaterFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, LogicalTypeId type_id) {

    ScalarFunction greater_function(
            ">",
            { LogicalType(type_id), LogicalType(type_id) },
            { LogicalType(LogicalTypeId::kBoolean) },
            &ScalarFunction::BinaryFunction<DataType, DataType, bool, GreaterFunction>);
    function_set_ptr->AddFunction(greater_function);
}

void
RegisterGreaterFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(">");

    GenerateGreaterFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    GenerateGreaterFunction<int8_t>(function_set_ptr, LogicalTypeId::kTinyInt);
    GenerateGreaterFunction<int16_t>(function_set_ptr, LogicalTypeId::kSmallInt);
    GenerateGreaterFunction<int32_t>(function_set_ptr, LogicalTypeId::kInteger);
    GenerateGreaterFunction<int64_t>(function_set_ptr, LogicalTypeId::kBigInt);
    GenerateGreaterFunction<float>(function_set_ptr, LogicalTypeId::kFloat);
    GenerateGreaterFunction<double>(function_set_ptr, LogicalTypeId::kDouble);

    // TODO: need to review following function
    GenerateGreaterFunction<int64_t>(function_set_ptr, LogicalTypeId::kDate);
    GenerateGreaterFunction<int64_t>(function_set_ptr, LogicalTypeId::kInterval);
    GenerateGreaterFunction<int64_t>(function_set_ptr, LogicalTypeId::kDateTime);
    GenerateGreaterFunction<int64_t>(function_set_ptr, LogicalTypeId::kTime);

    // FIXME: decimal isn't int64
    GenerateGreaterFunction<int64_t>(function_set_ptr, LogicalTypeId::kDecimal);

    // TODO: need to review following function
    GenerateGreaterFunction<std::string>(function_set_ptr, LogicalTypeId::kVarchar);
    GenerateGreaterFunction<std::string>(function_set_ptr, LogicalTypeId::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
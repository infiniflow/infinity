//
// Created by JinHai on 2022/9/28.
//

#include "notequals.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct NotEqualsFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left != right;
    }
};

template<typename DataType>
static void
GenerateNotEqualsFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, LogicalTypeId type_id) {

    ScalarFunction not_equals_function(
            "<>",
            { LogicalType(type_id), LogicalType(type_id) },
            { LogicalType(LogicalTypeId::kBoolean) },
            &ScalarFunction::BinaryFunction<DataType, DataType, bool, NotEqualsFunction>);
    function_set_ptr->AddFunction(not_equals_function);
}

void
RegisterNotEqualsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("<>");

    GenerateNotEqualsFunction<bool>(function_set_ptr, LogicalTypeId::kBoolean);
    GenerateNotEqualsFunction<int8_t>(function_set_ptr, LogicalTypeId::kTinyInt);
    GenerateNotEqualsFunction<int16_t>(function_set_ptr, LogicalTypeId::kSmallInt);
    GenerateNotEqualsFunction<int32_t>(function_set_ptr, LogicalTypeId::kInteger);
    GenerateNotEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kBigInt);
    GenerateNotEqualsFunction<float>(function_set_ptr, LogicalTypeId::kFloat);
    GenerateNotEqualsFunction<double>(function_set_ptr, LogicalTypeId::kDouble);

    // TODO: need to review following function
    GenerateNotEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDate);
    GenerateNotEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kInterval);
    GenerateNotEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDateTime);
    GenerateNotEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kTime);

    // FIXME: decimal isn't int64
    GenerateNotEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDecimal);

    // TODO: need to review following function
    GenerateNotEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kVarchar);
    GenerateNotEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}

//
// Created by JinHai on 2022/9/28.
//

#include "less_equals.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct LessEqualsFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left > right;
    }
};

template<typename CompareType>
static void
GenerateLessEqualsFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, DataType data_type) {

    ScalarFunction less_equals_function(
            "<=",
            { data_type, data_type },
            DataType(LogicalType::kBoolean),
            &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, LessEqualsFunction>);
    function_set_ptr->AddFunction(less_equals_function);
}

void
RegisterLessEqualsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("<=");

    GenerateLessEqualsFunction<bool>(function_set_ptr, DataType(LogicalType::kBoolean));
    GenerateLessEqualsFunction<int8_t>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateLessEqualsFunction<int16_t>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateLessEqualsFunction<int32_t>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateLessEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateLessEqualsFunction<float>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateLessEqualsFunction<double>(function_set_ptr, DataType(LogicalType::kDouble));

    // TODO: finish other types of function
//    // TODO: need to review following function
//    GenerateLessEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDate);
//    GenerateLessEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kInterval);
//    GenerateLessEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDateTime);
//    GenerateLessEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kTime);
//
//    // FIXME: decimal isn't int64
//    GenerateLessEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDecimal);
//
//    // TODO: need to review following function
//    GenerateLessEqualsFunction<std::string>(function_set_ptr, DataType(LogicalType::kVarchar);
//    GenerateLessEqualsFunction<std::string>(function_set_ptr, DataType(LogicalType::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
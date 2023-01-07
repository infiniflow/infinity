//
// Created by JinHai on 2022/9/28.
//

#include "greater_equals.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct GreaterEqualsFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left > right;
    }
};

template<typename CompareType>
static void
GenerateGreaterEqualsFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, DataType data_type) {

    ScalarFunction greater_equals_function(
            ">=",
            { data_type, data_type },
            { DataType(LogicalType::kBoolean) },
            &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, GreaterEqualsFunction>);
    function_set_ptr->AddFunction(greater_equals_function);
}

void
RegisterGreaterEqualsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(">=");

    GenerateGreaterEqualsFunction<BooleanT>(function_set_ptr, DataType(LogicalType::kBoolean));
    GenerateGreaterEqualsFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateGreaterEqualsFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateGreaterEqualsFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateGreaterEqualsFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateGreaterEqualsFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateGreaterEqualsFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    // TODO: finish other type function
//    // TODO: need to review following function
//    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDate);
//    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kInterval);
//    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDateTime);
//    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kTime);
//
//    // FIXME: decimal isn't int64
//    GenerateGreaterEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDecimal);
//
//    // TODO: need to review following function
//    GenerateGreaterEqualsFunction<std::string>(function_set_ptr, DataType(LogicalType::kVarchar);
//    GenerateGreaterEqualsFunction<std::string>(function_set_ptr, DataType(LogicalType::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
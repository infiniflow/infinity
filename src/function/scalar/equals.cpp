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

template<typename CompareType>
static void
GenerateEqualsFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, DataType data_type) {

    ScalarFunction equals_function(
            "=",
            { data_type, data_type },
            DataType(kBoolean),
            &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, EqualsFunction>);
    function_set_ptr->AddFunction(equals_function);
}

void
RegisterEqualsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("=");

    GenerateEqualsFunction<BooleanT>(function_set_ptr, DataType(LogicalType::kBoolean));
    GenerateEqualsFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateEqualsFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateEqualsFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateEqualsFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateEqualsFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateEqualsFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    // TODO: other equals function
//    // TODO: need to review following function
//    GenerateEqualsFunction<DateT>(function_set_ptr, DataType(LogicalType::kDate));
//    GenerateEqualsFunction<IntervalT>(function_set_ptr, DataType(LogicalType::kInterval));
//    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDateTime);
//    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kTime);
//
//    // FIXME: decimal isn't int64
//    GenerateEqualsFunction<int64_t>(function_set_ptr, LogicalTypeId::kDecimal);
//
//    // TODO: need to review following function
//    GenerateEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kVarchar);
//    GenerateEqualsFunction<std::string>(function_set_ptr, LogicalTypeId::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}

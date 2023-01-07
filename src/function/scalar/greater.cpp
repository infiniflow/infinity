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
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left > right;
    }
};

template<typename CompareType>
static void
GenerateGreaterFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, DataType data_type) {

    ScalarFunction greater_function(
            ">",
            { data_type, data_type },
            DataType(LogicalType::kBoolean),
            &ScalarFunction::BinaryFunction<CompareType, CompareType, bool, GreaterFunction>);
    function_set_ptr->AddFunction(greater_function);
}

void
RegisterGreaterFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(">");

    GenerateGreaterFunction<BooleanT>(function_set_ptr, DataType(DataType(LogicalType::kBoolean)));
    GenerateGreaterFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateGreaterFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateGreaterFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateGreaterFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateGreaterFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateGreaterFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    // TODO: finish following type function
//    // TODO: need to review following function
//    GenerateGreaterFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDate);
//    GenerateGreaterFunction<int64_t>(function_set_ptr, DataType(LogicalType::kInterval);
//    GenerateGreaterFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDateTime);
//    GenerateGreaterFunction<int64_t>(function_set_ptr, DataType(LogicalType::kTime);
//
//    // FIXME: decimal isn't int64
//    GenerateGreaterFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDecimal);
//
//    // TODO: need to review following function
//    GenerateGreaterFunction<std::string>(function_set_ptr, DataType(LogicalType::kVarchar);
//    GenerateGreaterFunction<std::string>(function_set_ptr, DataType(LogicalType::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
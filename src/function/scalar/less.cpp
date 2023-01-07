//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct LessFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left < right;
    }
};

template<typename CompareType>
static void
GenerateLessFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, DataType data_type) {

    ScalarFunction less_function(
            "<",
            { data_type, data_type },
            { DataType(LogicalType::kBoolean) },
            &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, LessFunction>);
    function_set_ptr->AddFunction(less_function);
}

void
RegisterLessFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("<");

    GenerateLessFunction<BooleanT>(function_set_ptr, DataType(LogicalType::kBoolean));
    GenerateLessFunction<TinyIntT>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateLessFunction<SmallIntT>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateLessFunction<IntegerT>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateLessFunction<BigIntT>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateLessFunction<FloatT>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateLessFunction<DoubleT>(function_set_ptr, DataType(LogicalType::kDouble));

    // TODO: finish following function
//    // TODO: need to review following function
//    GenerateLessFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDate);
//    GenerateLessFunction<int64_t>(function_set_ptr, DataType(LogicalType::kInterval);
//    GenerateLessFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDateTime);
//    GenerateLessFunction<int64_t>(function_set_ptr, DataType(LogicalType::kTime);
//
//    // FIXME: decimal isn't int64
//    GenerateLessFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDecimal);
//
//    // TODO: need to review following function
//    GenerateLessFunction<std::string>(function_set_ptr, DataType(LogicalType::kVarchar);
//    GenerateLessFunction<std::string>(function_set_ptr, DataType(LogicalType::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
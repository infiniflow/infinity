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
    static inline void
    Run(TB left, TC right, TC& result) {
        result = left != right;
    }
};

template<typename CompareType>
static void
GenerateNotEqualsFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr, DataType data_type) {

    ScalarFunction not_equals_function(
            "<>",
            { data_type, data_type },
            { DataType(LogicalType::kBoolean) },
            &ScalarFunction::BinaryFunction<CompareType, CompareType, BooleanT, NotEqualsFunction>);
    function_set_ptr->AddFunction(not_equals_function);
}

void
RegisterNotEqualsFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("<>");

    GenerateNotEqualsFunction<bool>(function_set_ptr, DataType(LogicalType::kBoolean));
    GenerateNotEqualsFunction<int8_t>(function_set_ptr, DataType(LogicalType::kTinyInt));
    GenerateNotEqualsFunction<int16_t>(function_set_ptr, DataType(LogicalType::kSmallInt));
    GenerateNotEqualsFunction<int32_t>(function_set_ptr, DataType(LogicalType::kInteger));
    GenerateNotEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kBigInt));
    GenerateNotEqualsFunction<float>(function_set_ptr, DataType(LogicalType::kFloat));
    GenerateNotEqualsFunction<double>(function_set_ptr, DataType(LogicalType::kDouble));

    // TODO: need to finish other functions
//    GenerateNotEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDate);
//    GenerateNotEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kInterval);
//    GenerateNotEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDateTime);
//    GenerateNotEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kTime);
//
//    // FIXME: decimal isn't int64
//    GenerateNotEqualsFunction<int64_t>(function_set_ptr, DataType(LogicalType::kDecimal);
//
//    // TODO: need to review following function
//    GenerateNotEqualsFunction<std::string>(function_set_ptr, DataType(LogicalType::kVarchar);
//    GenerateNotEqualsFunction<std::string>(function_set_ptr, DataType(LogicalType::kText);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}

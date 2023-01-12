//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct AndFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left and right;
    }
};

static void
GenerateAndFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr) {

    ScalarFunction and_function(
            "and",
            { DataType(LogicalType::kBoolean), DataType(LogicalType::kBoolean) },
            { DataType(LogicalType::kBoolean) },
            &ScalarFunction::BinaryFunction<BooleanT, BooleanT, BooleanT, AndFunction>);
    function_set_ptr->AddFunction(and_function);
}

void
RegisterAndFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("and");

    GenerateAndFunction(function_set_ptr);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
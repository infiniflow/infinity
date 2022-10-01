//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include <cmath>

namespace infinity {

struct OrFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left or right;
    }
};

static void
GenerateOrFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr) {

    ScalarFunction or_function(
            "or",
            { LogicalType(LogicalTypeId::kBoolean), LogicalType(LogicalTypeId::kBoolean) },
            { LogicalType(LogicalTypeId::kBoolean) },
            &ScalarFunction::BinaryFunction<bool, bool, bool, OrFunction>);
    function_set_ptr->AddFunction(or_function);
}

void
RegisterOrFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("or");

    GenerateOrFunction(function_set_ptr);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
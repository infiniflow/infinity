//
// Created by jinhai on 23-1-12.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct NotFunction {
    template<typename TA, typename TB>
    static inline void
    Run(TA input, TB& result) {
        result = !input;
    }
};

static void
GenerateNotFunction(std::shared_ptr<ScalarFunctionSet>& function_set_ptr) {

    ScalarFunction not_function(
            "not",
            { DataType(LogicalType::kBoolean) },
            { DataType(LogicalType::kBoolean) },
            &ScalarFunction::UnaryFunction<BooleanT, BooleanT, NotFunction>);
    function_set_ptr->AddFunction(not_function);
}

void
RegisterNotFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("not");

    GenerateNotFunction(function_set_ptr);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
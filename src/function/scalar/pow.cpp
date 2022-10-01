//
// Created by JinHai on 2022/9/16.
//

#include "pow.h"
#include "function/scalar_function.h"
#include <cmath>

namespace infinity {

struct PowFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB base, TC exponent) {
        return std::pow(base, exponent);
    }
};

void
RegisterPowFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("pow");

    ScalarFunction pow_function(
            "pow",
            { LogicalType(LogicalTypeId::kDouble), LogicalType(LogicalTypeId::kDouble) },
            { LogicalType(LogicalTypeId::kDouble) },
            &ScalarFunction::BinaryFunction<double, double, double, PowFunction>);
    function_set_ptr->AddFunction(pow_function);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}

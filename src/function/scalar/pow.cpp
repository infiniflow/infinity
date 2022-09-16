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

ScalarFunctionSet
RegisterPowFunction() {
    ScalarFunctionSet function_set("pow");

    ScalarFunction pow_function(
            "pow",
            { LogicalType(LogicalTypeId::kDouble), LogicalType(LogicalTypeId::kDouble) },
            { LogicalType(LogicalTypeId::kDouble) },
            &ScalarFunction::BinaryFunction<double, double, double, PowFunction>);
    function_set.AddFunction(pow_function);

    return function_set;
}

}
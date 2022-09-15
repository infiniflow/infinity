//
// Created by JinHai on 2022/9/14.
//

#include "abs.h"
#include "function/scalar_function.h"

namespace infinity {

struct AbsFunction {
    template <typename TA, typename TB>
    static inline TA Execute(TB value) {
        if(value == std::numeric_limits<TB>::min()) {
            ExecutorError("Overflow on abs(" + std::to_string(value) + ").");
        }
        return value < 0 ? -value : value;
    }
};


ScalarFunctionSet
RegisterAbsFunction() {
    ScalarFunctionSet function_set("abs");

    ScalarFunction abs_int8(
            "abs",
            { LogicalType(LogicalTypeId::kTinyInt) },
            { LogicalType(LogicalTypeId::kTinyInt) },
            &ScalarFunction::UnaryFunction<int8_t, int8_t, AbsFunction>);
    function_set.AddFunction(abs_int8);

    return function_set;
}

}
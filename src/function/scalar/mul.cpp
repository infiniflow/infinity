//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct MulFunction {
    template<typename TA, typename TB, typename TC>
    static inline TA Execute(TB left, TC right) {
        return left * right;
    }
};

void
RegisterMulFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("*");

    ScalarFunction mul_function_int64(
            "*",
            { LogicalType(LogicalTypeId::kBigInt), LogicalType(LogicalTypeId::kBigInt) },
            { LogicalType(LogicalTypeId::kBigInt) },
            &ScalarFunction::BinaryFunction<int64_t, int64_t, int64_t, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int64);

    ScalarFunction mul_function_double(
            "*",
            { LogicalType(LogicalTypeId::kDouble), LogicalType(LogicalTypeId::kDouble) },
            { LogicalType(LogicalTypeId::kDouble) },
            &ScalarFunction::BinaryFunction<double, double, double, MulFunction>);
    function_set_ptr->AddFunction(mul_function_double);

    // TODO: other type multiply need to be given.
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
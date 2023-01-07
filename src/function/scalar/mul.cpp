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
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left * right;
    }
};

void
RegisterMulFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("*");

    ScalarFunction mul_function_int64(
            "*",
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunction<BigIntT, BigIntT, BigIntT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_int64);

    ScalarFunction mul_function_double(
            "*",
            { DataType(LogicalType::kDouble), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunction<DoubleT, DoubleT, DoubleT, MulFunction>);
    function_set_ptr->AddFunction(mul_function_double);

    // TODO: other type multiply need to be given.
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
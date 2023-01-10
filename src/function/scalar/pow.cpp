//
// Created by JinHai on 2022/9/16.
//

#include "pow.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct PowFunction {
    template<typename TA, typename TB, typename TC>
    static inline bool
    Run(TA base, TB exponent, TC& result) {
        result = std::pow(base, exponent);
        return true;
    }
};

template<>
inline bool
PowFunction::Run(MixedT base, DoubleT exponent, DoubleT& result) {
    NotImplementError("Not implement pow(heterogeneous, double)")
}

void
RegisterPowFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("pow");

    ScalarFunction pow_function_float(
            "pow",
            { DataType(LogicalType::kFloat), DataType(LogicalType::kFloat) },
            { DataType(LogicalType::kFloat) },
            &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_float);

    ScalarFunction pow_function_double(
            "pow",
            { DataType(LogicalType::kDouble), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_double);

    ScalarFunction pow_function_mixed_double(
            "pow",
            { DataType(LogicalType::kMixed), DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, DoubleT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_mixed_double);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}

//
// Created by JinHai on 2022/9/16.
//

module;

#include <cmath>

import stl;
import new_catalog;
import infinity_assert;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module pow;

namespace infinity {

struct PowFunction {
    template <typename TA, typename TB, typename TC>
    static inline bool Run(TA base, TB exponent, TC &result) {
        result = std::pow(base, exponent);
        return true;
    }
};

template <>
inline bool PowFunction::Run(MixedT base, DoubleT exponent, DoubleT &result) {
    Error<NotImplementException>("Not implement", __FILE_NAME__, __LINE__);
}

void RegisterPowFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "POW";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction pow_function_float(func_name,
                                      {DataType(LogicalType::kFloat), DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kFloat)},
                                      &ScalarFunction::BinaryFunctionWithFailure<FloatT, FloatT, FloatT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_float);

    ScalarFunction pow_function_double(func_name,
                                       {DataType(LogicalType::kDouble), DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kDouble)},
                                       &ScalarFunction::BinaryFunctionWithFailure<DoubleT, DoubleT, DoubleT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_double);

    ScalarFunction pow_function_mixed_double(func_name,
                                             {DataType(LogicalType::kMixed), DataType(LogicalType::kDouble)},
                                             {DataType(LogicalType::kDouble)},
                                             &ScalarFunction::BinaryFunctionWithFailure<MixedT, DoubleT, DoubleT, PowFunction>);
    function_set_ptr->AddFunction(pow_function_mixed_double);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

module;

#include <cmath>

module sqrt;

import stl;
import catalog;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import internal_types;
import data_type;

namespace infinity {

struct SqrtFunction {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType value, TargetType &result) {
        if (value < static_cast<SourceType>(0.0f)) {
            return false;
        }
        result = sqrt(static_cast<double>(value));
        return true;
    }
};

void RegisterSqrtFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "sqrt";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction sqrt_int8(func_name,
                            {DataType(LogicalType::kTinyInt)},
                            DataType(LogicalType::kDouble),
                            &ScalarFunction::UnaryFunctionWithFailure<TinyIntT, DoubleT, SqrtFunction>);
    function_set_ptr->AddFunction(sqrt_int8);

    ScalarFunction sqrt_int16(func_name,
                             {DataType(LogicalType::kSmallInt)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<SmallIntT, DoubleT, SqrtFunction>);
    function_set_ptr->AddFunction(sqrt_int16);

    ScalarFunction sqrt_int32(func_name,
                             {DataType(LogicalType::kInteger)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<IntegerT, DoubleT, SqrtFunction>);
    function_set_ptr->AddFunction(sqrt_int32);

    ScalarFunction sqrt_int64(func_name,
                             {DataType(LogicalType::kBigInt)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<BigIntT, DoubleT, SqrtFunction>);
    function_set_ptr->AddFunction(sqrt_int64);

    ScalarFunction sqrt_float(func_name,
                             {DataType(LogicalType::kFloat)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<FloatT, DoubleT, SqrtFunction>);
    function_set_ptr->AddFunction(sqrt_float);

    ScalarFunction sqrt_double(func_name,
                              {DataType(LogicalType::kDouble)},
                              {DataType(LogicalType::kDouble)},
                              &ScalarFunction::UnaryFunctionWithFailure<DoubleT, DoubleT, SqrtFunction>);
    function_set_ptr->AddFunction(sqrt_double);

    ScalarFunction sqrt_float16(func_name,
                               {DataType(LogicalType::kFloat16)},
                               {DataType(LogicalType::kDouble)},
                               &ScalarFunction::UnaryFunctionWithFailure<Float16T, DoubleT, SqrtFunction>);
    function_set_ptr->AddFunction(sqrt_float16);

    ScalarFunction sqrt_bfloat16(func_name,
                                {DataType(LogicalType::kBFloat16)},
                                {DataType(LogicalType::kDouble)},
                                &ScalarFunction::UnaryFunctionWithFailure<BFloat16T, DoubleT, SqrtFunction>);
    function_set_ptr->AddFunction(sqrt_bfloat16);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
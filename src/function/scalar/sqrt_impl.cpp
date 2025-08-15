module;

module infinity_core:sqrt.impl;

import :sqrt;
import :stl;
import :new_catalog;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import std.compat;

import internal_types;
import data_type;
import logical_type;

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

void RegisterSqrtFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "sqrt";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

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

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
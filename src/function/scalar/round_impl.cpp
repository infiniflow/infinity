module infinity_core:round.impl;

import :round;
import :new_catalog;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import std.compat;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

struct RoundFunctionInt {
    template <typename SourceType, typename TargetType>
    static inline void Run(SourceType value, TargetType &result) {
        result = value;
    }
};

struct RoundFunctionFloat {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType value, TargetType &result) {
        result = round(static_cast<double>(value));
        if (std::isnan(result) || std::isinf(result)) {
            return false;
        }
        return true;
    }
};

void RegisterRoundFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "round";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction round_int8(func_name,
                              {DataType(LogicalType::kTinyInt)},
                              DataType(LogicalType::kTinyInt),
                              &ScalarFunction::UnaryFunction<TinyIntT, TinyIntT, RoundFunctionInt>);
    function_set_ptr->AddFunction(round_int8);

    ScalarFunction round_int16(func_name,
                               {DataType(LogicalType::kSmallInt)},
                               {DataType(LogicalType::kSmallInt)},
                               &ScalarFunction::UnaryFunction<SmallIntT, SmallIntT, RoundFunctionInt>);
    function_set_ptr->AddFunction(round_int16);

    ScalarFunction round_int32(func_name,
                               {DataType(LogicalType::kInteger)},
                               {DataType(LogicalType::kInteger)},
                               &ScalarFunction::UnaryFunction<IntegerT, IntegerT, RoundFunctionInt>);
    function_set_ptr->AddFunction(round_int32);

    ScalarFunction round_int64(func_name,
                               {DataType(LogicalType::kBigInt)},
                               {DataType(LogicalType::kBigInt)},
                               &ScalarFunction::UnaryFunction<BigIntT, BigIntT, RoundFunctionInt>);
    function_set_ptr->AddFunction(round_int64);

    ScalarFunction round_float(func_name,
                               {DataType(LogicalType::kFloat)},
                               {DataType(LogicalType::kDouble)},
                               &ScalarFunction::UnaryFunctionWithFailure<FloatT, DoubleT, RoundFunctionFloat>);
    function_set_ptr->AddFunction(round_float);

    ScalarFunction round_double(func_name,
                                {DataType(LogicalType::kDouble)},
                                {DataType(LogicalType::kDouble)},
                                &ScalarFunction::UnaryFunctionWithFailure<DoubleT, DoubleT, RoundFunctionFloat>);
    function_set_ptr->AddFunction(round_double);

    ScalarFunction round_float16(func_name,
                                 {DataType(LogicalType::kFloat16)},
                                 {DataType(LogicalType::kDouble)},
                                 &ScalarFunction::UnaryFunctionWithFailure<Float16T, DoubleT, RoundFunctionFloat>);
    function_set_ptr->AddFunction(round_float16);

    ScalarFunction round_bfloat16(func_name,
                                  {DataType(LogicalType::kBFloat16)},
                                  {DataType(LogicalType::kDouble)},
                                  &ScalarFunction::UnaryFunctionWithFailure<BFloat16T, DoubleT, RoundFunctionFloat>);
    function_set_ptr->AddFunction(round_bfloat16);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
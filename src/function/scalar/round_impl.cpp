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
        result = std::round(static_cast<double>(value));
        if (std::isnan(result) || std::isinf(result)) {
            return false;
        }
        return true;
    }
};

struct RoundFunctionFloatWithPrecision {
    template <typename SourceType, typename PrecisionType, typename TargetType>
    static inline bool Run(SourceType value, PrecisionType precision, TargetType &result) {
        double val = static_cast<double>(value);
        if (precision == 0) {
            result = std::round(val);
        } else if (precision > 0) {
            if (precision > 308) {
                result = val;
                return true;
            }
            double scale = std::pow(10.0, static_cast<double>(precision));
            result = std::round(val * scale) / scale;
        } else {
            if (precision < -308) {
                result = 0.0;
                return true;
            }
            double scale = std::pow(10.0, static_cast<double>(-precision));
            result = std::round(val / scale) * scale;
        }

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

    ScalarFunction round_int8_2(func_name,
                                {DataType(LogicalType::kTinyInt), DataType(LogicalType::kBigInt)},
                                DataType(LogicalType::kDouble),
                                &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, BigIntT, DoubleT, RoundFunctionFloatWithPrecision>);
    function_set_ptr->AddFunction(round_int8_2);

    ScalarFunction round_int16_2(func_name,
                                 {DataType(LogicalType::kSmallInt), DataType(LogicalType::kBigInt)},
                                 DataType(LogicalType::kDouble),
                                 &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, BigIntT, DoubleT, RoundFunctionFloatWithPrecision>);
    function_set_ptr->AddFunction(round_int16_2);

    ScalarFunction round_int32_2(func_name,
                                 {DataType(LogicalType::kInteger), DataType(LogicalType::kBigInt)},
                                 DataType(LogicalType::kDouble),
                                 &ScalarFunction::BinaryFunctionWithFailure<IntegerT, BigIntT, DoubleT, RoundFunctionFloatWithPrecision>);
    function_set_ptr->AddFunction(round_int32_2);

    ScalarFunction round_int64_2(func_name,
                                 {DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt)},
                                 DataType(LogicalType::kDouble),
                                 &ScalarFunction::BinaryFunctionWithFailure<BigIntT, BigIntT, DoubleT, RoundFunctionFloatWithPrecision>);
    function_set_ptr->AddFunction(round_int64_2);

    ScalarFunction round_float_2(func_name,
                                 {DataType(LogicalType::kFloat), DataType(LogicalType::kBigInt)},
                                 DataType(LogicalType::kDouble),
                                 &ScalarFunction::BinaryFunctionWithFailure<FloatT, BigIntT, DoubleT, RoundFunctionFloatWithPrecision>);
    function_set_ptr->AddFunction(round_float_2);

    ScalarFunction round_double_2(func_name,
                                  {DataType(LogicalType::kDouble), DataType(LogicalType::kBigInt)},
                                  DataType(LogicalType::kDouble),
                                  &ScalarFunction::BinaryFunctionWithFailure<DoubleT, BigIntT, DoubleT, RoundFunctionFloatWithPrecision>);
    function_set_ptr->AddFunction(round_double_2);

    ScalarFunction round_float16_2(func_name,
                                   {DataType(LogicalType::kFloat16), DataType(LogicalType::kBigInt)},
                                   DataType(LogicalType::kDouble),
                                   &ScalarFunction::BinaryFunctionWithFailure<Float16T, BigIntT, DoubleT, RoundFunctionFloatWithPrecision>);
    function_set_ptr->AddFunction(round_float16_2);

    ScalarFunction round_bfloat16_2(func_name,
                                    {DataType(LogicalType::kBFloat16), DataType(LogicalType::kBigInt)},
                                    DataType(LogicalType::kDouble),
                                    &ScalarFunction::BinaryFunctionWithFailure<BFloat16T, BigIntT, DoubleT, RoundFunctionFloatWithPrecision>);
    function_set_ptr->AddFunction(round_bfloat16_2);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
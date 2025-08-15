module infinity_core:floor.impl;

import :floor;
import :new_catalog;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import std.compat;

import internal_types;
import data_type;
import logical_type;

namespace infinity {

struct FloorFunctionInt {
    template <typename SourceType, typename TargetType>
    static inline void Run(SourceType value, TargetType &result) {
        result = value;
    }
};

struct FloorFunctionFloat {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType value, TargetType &result) {
        result = floor(static_cast<double>(value));
        if (std::isnan(result) || std::isinf(result)) {
            return false;
        }
        return true;
    }
};

void RegisterFloorFunction(NewCatalog *catalog_ptr) {
    std::string func_name = "floor";

    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>(func_name);

    ScalarFunction floor_int8(func_name,
                              {DataType(LogicalType::kTinyInt)},
                              DataType(LogicalType::kTinyInt),
                              &ScalarFunction::UnaryFunction<TinyIntT, TinyIntT, FloorFunctionInt>);
    function_set_ptr->AddFunction(floor_int8);

    ScalarFunction floor_int16(func_name,
                               {DataType(LogicalType::kSmallInt)},
                               {DataType(LogicalType::kSmallInt)},
                               &ScalarFunction::UnaryFunction<SmallIntT, SmallIntT, FloorFunctionInt>);
    function_set_ptr->AddFunction(floor_int16);

    ScalarFunction floor_int32(func_name,
                               {DataType(LogicalType::kInteger)},
                               {DataType(LogicalType::kInteger)},
                               &ScalarFunction::UnaryFunction<IntegerT, IntegerT, FloorFunctionInt>);
    function_set_ptr->AddFunction(floor_int32);

    ScalarFunction floor_int64(func_name,
                               {DataType(LogicalType::kBigInt)},
                               {DataType(LogicalType::kBigInt)},
                               &ScalarFunction::UnaryFunction<BigIntT, BigIntT, FloorFunctionInt>);
    function_set_ptr->AddFunction(floor_int64);

    ScalarFunction floor_float(func_name,
                               {DataType(LogicalType::kFloat)},
                               {DataType(LogicalType::kDouble)},
                               &ScalarFunction::UnaryFunctionWithFailure<FloatT, DoubleT, FloorFunctionFloat>);
    function_set_ptr->AddFunction(floor_float);

    ScalarFunction floor_double(func_name,
                                {DataType(LogicalType::kDouble)},
                                {DataType(LogicalType::kDouble)},
                                &ScalarFunction::UnaryFunctionWithFailure<DoubleT, DoubleT, FloorFunctionFloat>);
    function_set_ptr->AddFunction(floor_double);

    ScalarFunction floor_float16(func_name,
                                 {DataType(LogicalType::kFloat16)},
                                 {DataType(LogicalType::kDouble)},
                                 &ScalarFunction::UnaryFunctionWithFailure<Float16T, DoubleT, FloorFunctionFloat>);
    function_set_ptr->AddFunction(floor_float16);

    ScalarFunction floor_bfloat16(func_name,
                                  {DataType(LogicalType::kBFloat16)},
                                  {DataType(LogicalType::kDouble)},
                                  &ScalarFunction::UnaryFunctionWithFailure<BFloat16T, DoubleT, FloorFunctionFloat>);
    function_set_ptr->AddFunction(floor_bfloat16);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
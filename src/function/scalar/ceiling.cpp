module;

#include <cmath>

module ceiling;

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

struct CeilingFunctionInt {
    template <typename SourceType, typename TargetType>
    static inline void Run(SourceType value, TargetType &result) {
        result = value;
    }
};

struct CeilingFunctionFloat {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType value, TargetType &result) {
        result = ceil(static_cast<double>(value));
        if (std::isnan(result) || std::isinf(result)) {
            return false;
        }
        return true;
    }
};

void RegisterCeilingFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "ceiling";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction ceiling_int8(func_name,
                            {DataType(LogicalType::kTinyInt)},
                            DataType(LogicalType::kTinyInt),
                            &ScalarFunction::UnaryFunction<TinyIntT, TinyIntT, CeilingFunctionInt>);
    function_set_ptr->AddFunction(ceiling_int8);

    ScalarFunction ceiling_int16(func_name,
                             {DataType(LogicalType::kSmallInt)},
                             {DataType(LogicalType::kSmallInt)},
                             &ScalarFunction::UnaryFunction<SmallIntT, SmallIntT, CeilingFunctionInt>);
    function_set_ptr->AddFunction(ceiling_int16);

    ScalarFunction ceiling_int32(func_name,
                             {DataType(LogicalType::kInteger)},
                             {DataType(LogicalType::kInteger)},
                             &ScalarFunction::UnaryFunction<IntegerT, IntegerT, CeilingFunctionInt>);
    function_set_ptr->AddFunction(ceiling_int32);

    ScalarFunction ceiling_int64(func_name,
                             {DataType(LogicalType::kBigInt)},
                             {DataType(LogicalType::kBigInt)},
                             &ScalarFunction::UnaryFunction<BigIntT, BigIntT, CeilingFunctionInt>);
    function_set_ptr->AddFunction(ceiling_int64);

    ScalarFunction ceiling_float(func_name,
                             {DataType(LogicalType::kFloat)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<FloatT, DoubleT, CeilingFunctionFloat>);
    function_set_ptr->AddFunction(ceiling_float);

    ScalarFunction ceiling_double(func_name,
                              {DataType(LogicalType::kDouble)},
                              {DataType(LogicalType::kDouble)},
                              &ScalarFunction::UnaryFunctionWithFailure<DoubleT, DoubleT, CeilingFunctionFloat>);
    function_set_ptr->AddFunction(ceiling_double);

    ScalarFunction ceiling_float16(func_name,
                               {DataType(LogicalType::kFloat16)},
                               {DataType(LogicalType::kDouble)},
                               &ScalarFunction::UnaryFunctionWithFailure<Float16T, DoubleT, CeilingFunctionFloat>);
    function_set_ptr->AddFunction(ceiling_float16);

    ScalarFunction ceiling_bfloat16(func_name,
                                {DataType(LogicalType::kBFloat16)},
                                {DataType(LogicalType::kDouble)},
                                &ScalarFunction::UnaryFunctionWithFailure<BFloat16T, DoubleT, CeilingFunctionFloat>);
    function_set_ptr->AddFunction(ceiling_bfloat16);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
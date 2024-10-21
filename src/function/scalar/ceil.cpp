module;

#include <cmath>

module ceil;

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

struct CeilFunctionInt {
    template <typename SourceType, typename TargetType>
    static inline void Run(SourceType value, TargetType &result) {
        result = value;
    }
};

struct CeilFunctionFloat {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType value, TargetType &result) {
        result = ceil(static_cast<double>(value));
        if (std::isnan(result) || std::isinf(result)) {
            return false;
        }
        return true;
    }
};

void RegisterCeilFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "ceil";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction Ceil_int8(func_name,
                            {DataType(LogicalType::kTinyInt)},
                            DataType(LogicalType::kTinyInt),
                            &ScalarFunction::UnaryFunction<TinyIntT, TinyIntT, CeilFunctionInt>);
    function_set_ptr->AddFunction(Ceil_int8);

    ScalarFunction Ceil_int16(func_name,
                             {DataType(LogicalType::kSmallInt)},
                             {DataType(LogicalType::kSmallInt)},
                             &ScalarFunction::UnaryFunction<SmallIntT, SmallIntT, CeilFunctionInt>);
    function_set_ptr->AddFunction(Ceil_int16);

    ScalarFunction Ceil_int32(func_name,
                             {DataType(LogicalType::kInteger)},
                             {DataType(LogicalType::kInteger)},
                             &ScalarFunction::UnaryFunction<IntegerT, IntegerT, CeilFunctionInt>);
    function_set_ptr->AddFunction(Ceil_int32);

    ScalarFunction Ceil_int64(func_name,
                             {DataType(LogicalType::kBigInt)},
                             {DataType(LogicalType::kBigInt)},
                             &ScalarFunction::UnaryFunction<BigIntT, BigIntT, CeilFunctionInt>);
    function_set_ptr->AddFunction(Ceil_int64);

    ScalarFunction Ceil_float(func_name,
                             {DataType(LogicalType::kFloat)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<FloatT, DoubleT, CeilFunctionFloat>);
    function_set_ptr->AddFunction(Ceil_float);

    ScalarFunction Ceil_double(func_name,
                              {DataType(LogicalType::kDouble)},
                              {DataType(LogicalType::kDouble)},
                              &ScalarFunction::UnaryFunctionWithFailure<DoubleT, DoubleT, CeilFunctionFloat>);
    function_set_ptr->AddFunction(Ceil_double);

    ScalarFunction Ceil_float16(func_name,
                               {DataType(LogicalType::kFloat16)},
                               {DataType(LogicalType::kDouble)},
                               &ScalarFunction::UnaryFunctionWithFailure<Float16T, DoubleT, CeilFunctionFloat>);
    function_set_ptr->AddFunction(Ceil_float16);

    ScalarFunction Ceil_bfloat16(func_name,
                                {DataType(LogicalType::kBFloat16)},
                                {DataType(LogicalType::kDouble)},
                                &ScalarFunction::UnaryFunctionWithFailure<BFloat16T, DoubleT, CeilFunctionFloat>);
    function_set_ptr->AddFunction(Ceil_bfloat16);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
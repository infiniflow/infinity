//
// Created by JinHai on 2022/9/14.
//

#include "abs.h"
#include "function_template.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"

namespace infinity {

struct AbsFunctionInt {
    template <typename SourceType, typename TargetType>
    static inline bool
    Run(SourceType value, TargetType& result) {
        if(value == std::numeric_limits<SourceType>::min()) {
            return false;
        }
        result = value < 0 ? -value : value;
        return true;
    }
};

struct AbsFunctionFloat {
    template <typename SourceType, typename TargetType>
    static inline void
    Run(SourceType value, TargetType& result) {
        result = value < 0 ? -value : value;
    }
};


void
RegisterAbsFunction(const UniquePtr<Catalog> &catalog_ptr) {
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>("abs");

    ScalarFunction abs_int8(
            "abs",
            { DataType(LogicalType::kTinyInt) },
            DataType(LogicalType::kTinyInt),
            &ScalarFunction::UnaryFunctionWithFailure<TinyIntT, TinyIntT, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int8);

    ScalarFunction abs_int16(
            "abs",
            { DataType(LogicalType::kSmallInt) },
            { DataType(LogicalType::kSmallInt) },
            &ScalarFunction::UnaryFunctionWithFailure<SmallIntT, SmallIntT, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int16);

    ScalarFunction abs_int32(
            "abs",
            { DataType(LogicalType::kInteger) },
            { DataType(LogicalType::kInteger) },
            &ScalarFunction::UnaryFunctionWithFailure<IntegerT, IntegerT, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int32);

    ScalarFunction abs_int64(
            "abs",
            { DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::UnaryFunctionWithFailure<BigIntT, BigIntT, AbsFunctionInt>);
    function_set_ptr->AddFunction(abs_int64);

    ScalarFunction abs_float(
            "abs",
            { DataType(LogicalType::kFloat) },
            { DataType(LogicalType::kFloat) },
            &ScalarFunction::UnaryFunction<FloatT, FloatT, AbsFunctionFloat>);
    function_set_ptr->AddFunction(abs_float);

    ScalarFunction abs_double(
            "abs",
            { DataType(LogicalType::kDouble) },
            { DataType(LogicalType::kDouble) },
            &ScalarFunction::UnaryFunction<DoubleT, DoubleT, AbsFunctionFloat>);
    function_set_ptr->AddFunction(abs_double);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
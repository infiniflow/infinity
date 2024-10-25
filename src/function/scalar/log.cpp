module;

#include <cmath>

module log;

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

struct LogFunction {
    template <typename SourceType, typename TargetType>
    static inline bool Run(SourceType value, TargetType &result) {
        if (value < static_cast<SourceType>(0.0f)) {
            return false;
        }
        result = log(static_cast<double>(value));
        return true;
    }
};

void RegisterLogFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "ln";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction log_int8(func_name,
                            {DataType(LogicalType::kTinyInt)},
                            DataType(LogicalType::kDouble),
                            &ScalarFunction::UnaryFunctionWithFailure<TinyIntT, DoubleT, LogFunction>);
    function_set_ptr->AddFunction(log_int8);

    ScalarFunction log_int16(func_name,
                             {DataType(LogicalType::kSmallInt)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<SmallIntT, DoubleT, LogFunction>);
    function_set_ptr->AddFunction(log_int16);

    ScalarFunction log_int32(func_name,
                             {DataType(LogicalType::kInteger)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<IntegerT, DoubleT, LogFunction>);
    function_set_ptr->AddFunction(log_int32);

    ScalarFunction log_int64(func_name,
                             {DataType(LogicalType::kBigInt)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<BigIntT, DoubleT, LogFunction>);
    function_set_ptr->AddFunction(log_int64);

    ScalarFunction log_float(func_name,
                             {DataType(LogicalType::kFloat)},
                             {DataType(LogicalType::kDouble)},
                             &ScalarFunction::UnaryFunctionWithFailure<FloatT, DoubleT, LogFunction>);
    function_set_ptr->AddFunction(log_float);

    ScalarFunction log_double(func_name,
                              {DataType(LogicalType::kDouble)},
                              {DataType(LogicalType::kDouble)},
                              &ScalarFunction::UnaryFunctionWithFailure<DoubleT, DoubleT, LogFunction>);
    function_set_ptr->AddFunction(log_double);

    ScalarFunction log_float16(func_name,
                               {DataType(LogicalType::kFloat16)},
                               {DataType(LogicalType::kDouble)},
                               &ScalarFunction::UnaryFunctionWithFailure<Float16T, DoubleT, LogFunction>);
    function_set_ptr->AddFunction(log_float16);

    ScalarFunction log_bfloat16(func_name,
                                {DataType(LogicalType::kBFloat16)},
                                {DataType(LogicalType::kDouble)},
                                &ScalarFunction::UnaryFunctionWithFailure<BFloat16T, DoubleT, LogFunction>);
    function_set_ptr->AddFunction(log_bfloat16);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
module;

#include <cmath>

module truncate;

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

struct TruncateFunction {
    template <typename XType, typename DType, typename ResultType>
    static inline bool Run(XType x, DType d, ResultType &result) {
        int64_t D = static_cast<int64_t>(d);

        if (static_cast<DType>(D) != d) {
            return false;
        }

        if (D > 0) {
            double factor = std::pow(10.0, D);
            result = static_cast<ResultType>(std::trunc(static_cast<double>(x) * factor) / factor);
        } else if (D == 0) {
            result = static_cast<ResultType>(std::trunc(static_cast<double>(x)));
        } else {
            double factor = std::pow(10.0, -D);
            result = static_cast<ResultType>(std::trunc(static_cast<double>(x) / factor) * factor);
        }
        return true;
    }
};

void RegisterTruncateFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "truncate";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction truncate_int8(func_name,
                                 {DataType(LogicalType::kTinyInt), DataType(LogicalType::kInteger)},
                                 DataType(LogicalType::kDouble),
                                 &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int8);

    ScalarFunction truncate_int16(func_name,
                                  {DataType(LogicalType::kSmallInt), DataType(LogicalType::kInteger)},
                                  DataType(LogicalType::kDouble),
                                  &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int16);

    ScalarFunction truncate_int32(func_name,
                                  {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                  DataType(LogicalType::kDouble),
                                  &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int32);

    ScalarFunction truncate_int64(func_name,
                                  {DataType(LogicalType::kBigInt), DataType(LogicalType::kInteger)},
                                  DataType(LogicalType::kDouble),
                                  &ScalarFunction::BinaryFunctionWithFailure<BigIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int64);

    ScalarFunction truncate_float16(func_name,
                                    {DataType(LogicalType::kFloat16), DataType(LogicalType::kInteger)},
                                    DataType(LogicalType::kFloat16),
                                    &ScalarFunction::BinaryFunctionWithFailure<Float16T, IntegerT, Float16T, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_float16);

    ScalarFunction truncate_bfloat16(func_name,
                                     {DataType(LogicalType::kBFloat16), DataType(LogicalType::kInteger)},
                                     DataType(LogicalType::kBFloat16),
                                     &ScalarFunction::BinaryFunctionWithFailure<BFloat16T, IntegerT, BFloat16T, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_bfloat16);

    ScalarFunction truncate_float(func_name,
                                  {DataType(LogicalType::kFloat), DataType(LogicalType::kInteger)},
                                  DataType(LogicalType::kFloat),
                                  &ScalarFunction::BinaryFunctionWithFailure<FloatT, IntegerT, FloatT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_float);

    ScalarFunction truncate_double(func_name,
                                   {DataType(LogicalType::kDouble), DataType(LogicalType::kInteger)},
                                   DataType(LogicalType::kDouble),
                                   &ScalarFunction::BinaryFunctionWithFailure<DoubleT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_double);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

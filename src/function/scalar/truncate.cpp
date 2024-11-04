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
    static inline bool Run(XType x_value, DType d_value, ResultType &result) {
        double x_val = static_cast<double>(x_value);

        if (std::isnan(x_val) || std::isinf(x_val)) {
            return false;
        }

        int64_t d_int = static_cast<int64_t>(d_value);

        double factor = 1.0;
        double temp_result = 0.0;

        if (d_int > 0) {
            for (int64_t i = 0; i < d_int; ++i) {
                factor *= 10.0;
            }
            temp_result = std::trunc(x_val * factor) / factor;
        } else if (d_int < 0) {
            for (int64_t i = 0; i < -d_int; ++i) {
                factor *= 10.0;
            }
            temp_result = std::trunc(x_val / factor) * factor;
        } else {
            temp_result = std::trunc(x_val);
        }

        if (std::isnan(temp_result) || std::isinf(temp_result)) {
            return false;
        }

        result = static_cast<ResultType>(temp_result);
        return true;
    }
};

void RegisterTruncateFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "truncate";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);
    function_set_ptr->AddFunction(ScalarFunction(func_name,
                                                 {DataType(LogicalType::kTinyInt), DataType(LogicalType::kInteger)},
                                                 DataType(LogicalType::kDouble),
                                                 &ScalarFunction::BinaryFunctionWithFailure<TinyIntT, IntegerT, DoubleT, TruncateFunction>));

    function_set_ptr->AddFunction(ScalarFunction(func_name,
                                                 {DataType(LogicalType::kSmallInt), DataType(LogicalType::kInteger)},
                                                 DataType(LogicalType::kDouble),
                                                 &ScalarFunction::BinaryFunctionWithFailure<SmallIntT, IntegerT, DoubleT, TruncateFunction>));

    function_set_ptr->AddFunction(ScalarFunction(func_name,
                                                 {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                                 DataType(LogicalType::kDouble),
                                                 &ScalarFunction::BinaryFunctionWithFailure<IntegerT, IntegerT, DoubleT, TruncateFunction>));

    function_set_ptr->AddFunction(ScalarFunction(func_name,
                                                 {DataType(LogicalType::kBigInt), DataType(LogicalType::kInteger)},
                                                 DataType(LogicalType::kDouble),
                                                 &ScalarFunction::BinaryFunctionWithFailure<BigIntT, IntegerT, DoubleT, TruncateFunction>));

    function_set_ptr->AddFunction(ScalarFunction(func_name,
                                                 {DataType(LogicalType::kFloat16), DataType(LogicalType::kInteger)},
                                                 DataType(LogicalType::kFloat16),
                                                 &ScalarFunction::BinaryFunctionWithFailure<Float16T, IntegerT, Float16T, TruncateFunction>));

    function_set_ptr->AddFunction(ScalarFunction(func_name,
                                                 {DataType(LogicalType::kBFloat16), DataType(LogicalType::kInteger)},
                                                 DataType(LogicalType::kBFloat16),
                                                 &ScalarFunction::BinaryFunctionWithFailure<BFloat16T, IntegerT, BFloat16T, TruncateFunction>));

    function_set_ptr->AddFunction(ScalarFunction(func_name,
                                                 {DataType(LogicalType::kFloat), DataType(LogicalType::kInteger)},
                                                 DataType(LogicalType::kFloat),
                                                 &ScalarFunction::BinaryFunctionWithFailure<FloatT, IntegerT, FloatT, TruncateFunction>));

    function_set_ptr->AddFunction(ScalarFunction(func_name,
                                                 {DataType(LogicalType::kDouble), DataType(LogicalType::kInteger)},
                                                 DataType(LogicalType::kDouble),
                                                 &ScalarFunction::BinaryFunctionWithFailure<DoubleT, IntegerT, DoubleT, TruncateFunction>));

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

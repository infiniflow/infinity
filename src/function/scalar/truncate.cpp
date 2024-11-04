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
        // 检查 x_value 是否为 NaN 或 Inf
        if (std::isnan(x_value) || std::isinf(x_value)) {
            return false;
        }

        int64_t d_int = static_cast<int64_t>(d_value);

        // 优化：处理特殊情况，避免使用 std::pow
        double factor = 1.0;
        if (d_int > 0) {
            // 当 d_int > 0 时，计算 10 的 d_int 次方
            for (int64_t i = 0; i < d_int; ++i) {
                factor *= 10.0;
            }
            temp_result = std::trunc(x_value * factor) / factor;
        } else if (d_int < 0) {
            // 当 d_int < 0 时，计算 10 的 -d_int 次方
            for (int64_t i = 0; i < -d_int; ++i) {
                factor *= 10.0;
            }
            temp_result = std::trunc(x_value / factor) * factor;
        } else {
            // 当 d_int == 0 时，不需要计算
            temp_result = std::trunc(x_value);
        }

        // 检查结果是否有效
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

    // 注册不同数据类型的 truncate 函数
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

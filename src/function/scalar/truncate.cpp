#include <cmath>
#include <utility>

module truncate;

import stl;
import catalog;
import logical_type;
import scalar_function;
import scalar_function_set;

import third_party;
import internal_types;
import data_type;
import logger;

namespace infinity {

struct TruncateFunction {
    template <typename LeftType, typename RightType, typename OutputType>
    static inline bool Run(LeftType value, RightType decimals, OutputType &result) {
        if (decimals > 0) {
            double scale = pow(10.0, decimals);
            result = static_cast<OutputType>(std::trunc(value * scale) / scale);
        } else if (decimals == 0) {
            result = static_cast<OutputType>(std::trunc(value));
        } else {
            double scale = pow(10.0, -decimals);
            result = static_cast<OutputType>(std::trunc(value / scale) * scale);
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
                                 &ScalarFunction::BinaryFunction<TinyIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int8);

    ScalarFunction truncate_int16(func_name,
                                  {DataType(LogicalType::kSmallInt), DataType(LogicalType::kInteger)},
                                  DataType(LogicalType::kDouble),
                                  &ScalarFunction::BinaryFunction<SmallIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int16);

    ScalarFunction truncate_int32(func_name,
                                  {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                  DataType(LogicalType::kDouble),
                                  &ScalarFunction::BinaryFunction<IntegerT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int32);

    ScalarFunction truncate_int64(func_name,
                                  {DataType(LogicalType::kBigInt), DataType(LogicalType::kInteger)},
                                  DataType(LogicalType::kDouble),
                                  &ScalarFunction::BinaryFunction<BigIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int64);

    ScalarFunction truncate_float(func_name,
                                  {DataType(LogicalType::kFloat), DataType(LogicalType::kInteger)},
                                  DataType(LogicalType::kDouble),
                                  &ScalarFunction::BinaryFunction<FloatT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_float);

    ScalarFunction truncate_double(func_name,
                                   {DataType(LogicalType::kDouble), DataType(LogicalType::kInteger)},
                                   DataType(LogicalType::kDouble),
                                   &ScalarFunction::BinaryFunction<DoubleT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_double);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

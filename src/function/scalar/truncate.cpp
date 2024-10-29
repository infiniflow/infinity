module;

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
    static inline void Run(LeftType value, RightType decimals, OutputType &result) {
        if (decimals > 0) {
            double scale = pow(10.0, decimals);
            result = static_cast<OutputType>(std::trunc(value * scale) / scale);
        } else if (decimals == 0) {
            result = static_cast<OutputType>(std::trunc(value));
        } else {
            double scale = pow(10.0, -decimals);
            result = static_cast<OutputType>(std::trunc(value / scale) * scale);
        }
    }
};

void RegisterTruncateFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "truncate";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction truncate_int8_tinyint(func_name,
                                         {DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt)},
                                         DataType(LogicalType::kDouble),
                                         &ScalarFunction::BinaryFunction<TinyIntT, TinyIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int8_tinyint);

    ScalarFunction truncate_int8_smallint(func_name,
                                          {DataType(LogicalType::kTinyInt), DataType(LogicalType::kSmallInt)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<TinyIntT, SmallIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int8_smallint);

    ScalarFunction truncate_int8_integer(func_name,
                                         {DataType(LogicalType::kTinyInt), DataType(LogicalType::kInteger)},
                                         DataType(LogicalType::kDouble),
                                         &ScalarFunction::BinaryFunction<TinyIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int8_integer);

    ScalarFunction truncate_int16_tinyint(func_name,
                                          {DataType(LogicalType::kSmallInt), DataType(LogicalType::kTinyInt)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<SmallIntT, TinyIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int16_tinyint);

    ScalarFunction truncate_int16_smallint(func_name,
                                           {DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt)},
                                           DataType(LogicalType::kDouble),
                                           &ScalarFunction::BinaryFunction<SmallIntT, SmallIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int16_smallint);

    ScalarFunction truncate_int16_integer(func_name,
                                          {DataType(LogicalType::kSmallInt), DataType(LogicalType::kInteger)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<SmallIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int16_integer);

    ScalarFunction truncate_int32_tinyint(func_name,
                                          {DataType(LogicalType::kInteger), DataType(LogicalType::kTinyInt)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<IntegerT, TinyIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int32_tinyint);

    ScalarFunction truncate_int32_smallint(func_name,
                                           {DataType(LogicalType::kInteger), DataType(LogicalType::kSmallInt)},
                                           DataType(LogicalType::kDouble),
                                           &ScalarFunction::BinaryFunction<IntegerT, SmallIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int32_smallint);

    ScalarFunction truncate_int32_integer(func_name,
                                          {DataType(LogicalType::kInteger), DataType(LogicalType::kInteger)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<IntegerT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int32_integer);

    ScalarFunction truncate_int64_tinyint(func_name,
                                          {DataType(LogicalType::kBigInt), DataType(LogicalType::kTinyInt)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<BigIntT, TinyIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int64_tinyint);

    ScalarFunction truncate_int64_smallint(func_name,
                                           {DataType(LogicalType::kBigInt), DataType(LogicalType::kSmallInt)},
                                           DataType(LogicalType::kDouble),
                                           &ScalarFunction::BinaryFunction<BigIntT, SmallIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int64_smallint);

    ScalarFunction truncate_int64_integer(func_name,
                                          {DataType(LogicalType::kBigInt), DataType(LogicalType::kInteger)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<BigIntT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_int64_integer);

    ScalarFunction truncate_float_tinyint(func_name,
                                          {DataType(LogicalType::kFloat), DataType(LogicalType::kTinyInt)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<FloatT, TinyIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_float_tinyint);

    ScalarFunction truncate_float_smallint(func_name,
                                           {DataType(LogicalType::kFloat), DataType(LogicalType::kSmallInt)},
                                           DataType(LogicalType::kDouble),
                                           &ScalarFunction::BinaryFunction<FloatT, SmallIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_float_smallint);

    ScalarFunction truncate_float_integer(func_name,
                                          {DataType(LogicalType::kFloat), DataType(LogicalType::kInteger)},
                                          DataType(LogicalType::kDouble),
                                          &ScalarFunction::BinaryFunction<FloatT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_float_integer);

    ScalarFunction truncate_double_tinyint(func_name,
                                           {DataType(LogicalType::kDouble), DataType(LogicalType::kTinyInt)},
                                           DataType(LogicalType::kDouble),
                                           &ScalarFunction::BinaryFunction<DoubleT, TinyIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_double_tinyint);

    ScalarFunction truncate_double_smallint(func_name,
                                            {DataType(LogicalType::kDouble), DataType(LogicalType::kSmallInt)},
                                            DataType(LogicalType::kDouble),
                                            &ScalarFunction::BinaryFunction<DoubleT, SmallIntT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_double_smallint);

    ScalarFunction truncate_double_integer(func_name,
                                           {DataType(LogicalType::kDouble), DataType(LogicalType::kInteger)},
                                           DataType(LogicalType::kDouble),
                                           &ScalarFunction::BinaryFunction<DoubleT, IntegerT, DoubleT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_double_integer);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity

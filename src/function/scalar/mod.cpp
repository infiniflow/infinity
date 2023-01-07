//
// Created by JinHai on 2022/9/28.
//

#include "less.h"
#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct ModFunction {
    template<typename TA, typename TB, typename TC>
    static inline void
    Run(TA left, TB right, TC& result) {
        result = left % right;
    }
};

void
RegisterModFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("%");

    ScalarFunction mod_function_int8(
            "%",
            { DataType(LogicalType::kTinyInt), DataType(LogicalType::kTinyInt) },
            { DataType(LogicalType::kTinyInt) },
            &ScalarFunction::BinaryFunction<TinyIntT, TinyIntT, TinyIntT, ModFunction>);
    function_set_ptr->AddFunction(mod_function_int8);

    ScalarFunction mod_function_int16(
            "%",
            { DataType(LogicalType::kSmallInt), DataType(LogicalType::kSmallInt) },
            { DataType(LogicalType::kSmallInt) },
            &ScalarFunction::BinaryFunction<SmallIntT, SmallIntT, SmallIntT, ModFunction>);
    function_set_ptr->AddFunction(mod_function_int16);

    ScalarFunction mod_function_int32(
            "%",
            { DataType(LogicalType::kInteger), DataType(LogicalType::kInteger) },
            { DataType(LogicalType::kInteger) },
            &ScalarFunction::BinaryFunction<IntegerT, IntegerT, IntegerT, ModFunction>);
    function_set_ptr->AddFunction(mod_function_int32);

    ScalarFunction mod_function_int64(
            "%",
            { DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kBigInt) },
            &ScalarFunction::BinaryFunction<BigIntT, BigIntT, BigIntT, ModFunction>);
    function_set_ptr->AddFunction(mod_function_int64);

    // TODO: other type modulo need to be given.
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
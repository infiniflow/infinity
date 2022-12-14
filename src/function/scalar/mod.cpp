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
    static inline TA Execute(TB left, TC right) {
        return left % right;
    }
};

void
RegisterModFunction(const std::unique_ptr<Catalog> &catalog_ptr) {
    std::shared_ptr<ScalarFunctionSet> function_set_ptr = std::make_shared<ScalarFunctionSet>("%");

    ScalarFunction mod_function_int8(
            "%",
            { LogicalType(LogicalTypeId::kTinyInt), LogicalType(LogicalTypeId::kTinyInt) },
            { LogicalType(LogicalTypeId::kTinyInt) },
            &ScalarFunction::BinaryFunction<int64_t, int64_t, int64_t, ModFunction>);
    function_set_ptr->AddFunction(mod_function_int8);

    ScalarFunction mod_function_int16(
            "%",
            { LogicalType(LogicalTypeId::kSmallInt), LogicalType(LogicalTypeId::kSmallInt) },
            { LogicalType(LogicalTypeId::kSmallInt) },
            &ScalarFunction::BinaryFunction<int16_t, int16_t, int16_t, ModFunction>);
    function_set_ptr->AddFunction(mod_function_int16);

    ScalarFunction mod_function_int32(
            "%",
            { LogicalType(LogicalTypeId::kInteger), LogicalType(LogicalTypeId::kInteger) },
            { LogicalType(LogicalTypeId::kInteger) },
            &ScalarFunction::BinaryFunction<int32_t, int32_t, int32_t, ModFunction>);
    function_set_ptr->AddFunction(mod_function_int32);

    ScalarFunction mod_function_int64(
            "%",
            { LogicalType(LogicalTypeId::kBigInt), LogicalType(LogicalTypeId::kBigInt) },
            { LogicalType(LogicalTypeId::kBigInt) },
            &ScalarFunction::BinaryFunction<int64_t, int64_t, int64_t, ModFunction>);
    function_set_ptr->AddFunction(mod_function_int64);

    // TODO: other type modulo need to be given.
    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}
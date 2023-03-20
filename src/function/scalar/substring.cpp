//
// Created by jinhai on 23-3-20.
//

#include "substring.h"

#include "function/scalar_function.h"
#include "function/scalar_function_set.h"
#include <cmath>

namespace infinity {

struct SubstrFunction {
    template<typename TA, typename TB, typename TC, typename TD>
    static inline bool
    Run(TA first, TB second, TC third, TD& result, ColumnVector* column_vector_ptr) {
        NotImplementError("Not implemented");
    }
};

template<>
inline bool
SubstrFunction::Run(VarcharT first, BigIntT second, BigIntT third, VarcharT& result, ColumnVector* column_vector_ptr) {
    LOG_TRACE("{} from pos {} len {}", first.ToString(), second, third);
    return true;
}

template<>
inline bool
SubstrFunction::Run(CharT first, BigIntT second, BigIntT third, VarcharT& result, ColumnVector* column_vector_ptr) {
    LOG_TRACE("{} from pos {} len {}", first.ToString(), second, third);
    return true;
}

void
RegisterSubstringFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "substring";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction varchar_substr(
            func_name,
            { DataType(LogicalType::kVarchar), DataType(LogicalType::kBigInt), DataType(LogicalType::kBigInt) },
            { DataType(LogicalType::kVarchar) },
            &ScalarFunction::TernaryFunctionToVarlenWithFailure<VarcharT, BigIntT, BigIntT, VarcharT, SubstrFunction>);
    function_set_ptr->AddFunction(varchar_substr);

    catalog_ptr->AddFunctionSet(function_set_ptr);
}

}


module;
#include <type_traits>
#include "type/internal_types.h"
#include <ostream>
#include "type/logical_type.h"
#include <cstddef>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
module trunc;
import stl;
import catalog;
import status;
import logical_type;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import third_party;
import internal_types;
import data_type;
import column_vector;

namespace infinity {

struct TruncFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA left, TB right, TC &result, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void TruncFunction::Run(DoubleT left, BigIntT right, VarcharT &result, ColumnVector *result_ptr) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(16);
    ss << left;
    std::string str = ss.str();
    std::string truncated_str;
    size_t i = str.find_first_of('.');
    if (right < static_cast<BigIntT>(0) || std::isnan(right) || std::isinf(right)) {
        Status status = Status::InvalidDataType();
        RecoverableError(status);
        return;
    } else if (std::isnan(left)) {
        truncated_str = "NaN";
    } else if (std::isinf(left)) {
        truncated_str = "Inf";
    } else if (right > static_cast<BigIntT>(17) || static_cast<BigIntT>(str.size() - i) < right || right == static_cast<BigIntT>(0)) {
        truncated_str = str.substr(0, i);
    } else {
        truncated_str = str.substr(0, i + right + 1);
    }
    result_ptr->AppendVarcharInner(truncated_str, result);
}

void RegisterTruncFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "trunc";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction truncate_double_bigint(func_name,
                              {DataType(LogicalType::kDouble), DataType(LogicalType::kBigInt)},
                              DataType(LogicalType::kVarchar),
                              &ScalarFunction::BinaryFunctionToVarlen<DoubleT, BigIntT, VarcharT, TruncFunction>);
    function_set_ptr->AddFunction(truncate_double_bigint);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
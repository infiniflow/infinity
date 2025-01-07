
module;

#include <cstddef>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
module truncate;
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

struct TruncateFunction {
    template <typename TA, typename TB, typename TC, typename TD>
    static inline void Run(TA left, TB right, TC result, TD result_ptr) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};
template <>
inline void TruncateFunction::Run(FloatT left, IntegerT right, VarcharT &result, ColumnVector *result_ptr) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6);
    ss << left;
    std::string str = ss.str();
    size_t i = str.find_first_of('.');
    std::string truncated_str;
    if (right < static_cast<IntegerT>(0) || std::isnan(left) || std::isinf(left) || std::isnan(right) || std::isinf(right)) {
        truncated_str = str;
    } else if (right > static_cast<IntegerT>(7)  || str.size() - i < static_cast<size_t>(right)) {
        truncated_str = str.substr(0, i);
    } else {
        truncated_str = str.substr(0, i + right);
    }
    result_ptr->AppendVarcharInner(truncated_str, result);
}

// 
template <>
inline void TruncateFunction::Run(DoubleT left, IntegerT right, VarcharT &result, ColumnVector *result_ptr) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(16);
    ss << left;
    std::string str = ss.str();
    std::string truncated_str;
    size_t i = str.find_first_of('.');
    if (right < static_cast<IntegerT>(0) || std::isnan(left) || std::isinf(left) || std::isnan(right) || std::isinf(right)) {
        truncated_str = str;
    } else if (right > static_cast<IntegerT>(17) || str.size() - i < static_cast<size_t>(right)) {
    
        truncated_str = str.substr(0, i);
    } else {
        truncated_str = str.substr(0, i + right);
    }
    result_ptr->AppendVarcharInner(truncated_str, result);
}


void RegisterTruncateFunction(const UniquePtr<Catalog> &catalog_ptr) {
    String func_name = "truncate";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction truncate_float(func_name,
                              {DataType(LogicalType::kFloat), DataType(LogicalType::kInteger)},
                              DataType(LogicalType::kVarchar),
                              &ScalarFunction::BinaryFunctionToVarlen<FloatT, IntegerT, VarcharT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_float);

    ScalarFunction truncate_double(func_name,
                              {DataType(LogicalType::kDouble), DataType(LogicalType::kInteger)},
                              DataType(LogicalType::kVarchar),
                              &ScalarFunction::BinaryFunctionToVarlen<DoubleT, IntegerT, VarcharT, TruncateFunction>);
    function_set_ptr->AddFunction(truncate_double);


    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
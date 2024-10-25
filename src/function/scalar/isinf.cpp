module;

#include <cmath>

module isinf;

import stl;
import catalog;
import status;
import infinity_exception;
import scalar_function;
import scalar_function_set;

import third_party;
import logical_type;
import internal_types;
import data_type;
import logger;

namespace infinity {

struct IsinfFunction {
    template <typename TA, typename TB>
    static inline void Run(TA &left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};


template <>
inline void IsinfFunction::Run(FloatT &left, BooleanT &result) {
    result = std::isinf(left);
}

template <>
inline void IsinfFunction::Run(DoubleT &left, BooleanT &result) {
    result = std::isinf(left);
}

template <>
inline void IsinfFunction::Run(Float16T &left, BooleanT &result) {
    result = std::isinf(static_cast<float>(left));
}

template <>
inline void IsinfFunction::Run(BFloat16T &left, BooleanT &result) {
    result = std::isinf(static_cast<float>(left));
}


void RegisterIsinfFunction(const UniquePtr<Catalog> &catalog_ptr){
    String func_name = "isinf";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction isinf_function_float(func_name,
                                      {DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kBoolean)},
                                      &ScalarFunction::UnaryFunction<FloatT, BooleanT, IsinfFunction>);
    function_set_ptr->AddFunction(isinf_function_float);

    ScalarFunction isinf_function_double(func_name,
                                       {DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kBoolean)},
                                       &ScalarFunction::UnaryFunction<DoubleT, BooleanT, IsinfFunction>);
    function_set_ptr->AddFunction(isinf_function_double);

    ScalarFunction isinf_function_float16(func_name,
                                        {DataType(LogicalType::kFloat16)},
                                        {DataType(LogicalType::kBoolean)},
                                        &ScalarFunction::UnaryFunction<Float16T, BooleanT, IsinfFunction>);
    function_set_ptr->AddFunction(isinf_function_float16);

    ScalarFunction isinf_function_bfloat16(func_name,
                                         {DataType(LogicalType::kBFloat16)},
                                         {DataType(LogicalType::kBoolean)},
                                         &ScalarFunction::UnaryFunction<BFloat16T, BooleanT, IsinfFunction>);
    function_set_ptr->AddFunction(isinf_function_bfloat16);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
module;

module infinity_core:isfinite.impl;

import :isfinite;
import :stl;
import :new_catalog;
import :status;
import :infinity_exception;
import :scalar_function;
import :scalar_function_set;

import std.compat;

import logical_type;
import internal_types;
import data_type;

namespace infinity {

struct IsfiniteFunction {
    template <typename TA, typename TB>
    static inline void Run(TA &left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
    }
};

template <>
inline void IsfiniteFunction::Run(FloatT &left, BooleanT &result) {
    result = !std::isinf(left);
}

template <>
inline void IsfiniteFunction::Run(DoubleT &left, BooleanT &result) {
    result = !std::isinf(left);
}

template <>
inline void IsfiniteFunction::Run(Float16T &left, BooleanT &result) {
    result = !std::isinf(static_cast<float>(left));
}

template <>
inline void IsfiniteFunction::Run(BFloat16T &left, BooleanT &result) {
    result = !std::isinf(static_cast<float>(left));
}

void RegisterIsfiniteFunction(NewCatalog *catalog_ptr) {
    String func_name = "isfinite";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction isfinite_function_float(func_name,
                                           {DataType(LogicalType::kFloat)},
                                           {DataType(LogicalType::kBoolean)},
                                           &ScalarFunction::UnaryFunction<FloatT, BooleanT, IsfiniteFunction>);
    function_set_ptr->AddFunction(isfinite_function_float);

    ScalarFunction isfinite_function_double(func_name,
                                            {DataType(LogicalType::kDouble)},
                                            {DataType(LogicalType::kBoolean)},
                                            &ScalarFunction::UnaryFunction<DoubleT, BooleanT, IsfiniteFunction>);
    function_set_ptr->AddFunction(isfinite_function_double);

    ScalarFunction isfinite_function_float16(func_name,
                                             {DataType(LogicalType::kFloat16)},
                                             {DataType(LogicalType::kBoolean)},
                                             &ScalarFunction::UnaryFunction<Float16T, BooleanT, IsfiniteFunction>);
    function_set_ptr->AddFunction(isfinite_function_float16);

    ScalarFunction isfinite_function_bfloat16(func_name,
                                              {DataType(LogicalType::kBFloat16)},
                                              {DataType(LogicalType::kBoolean)},
                                              &ScalarFunction::UnaryFunction<BFloat16T, BooleanT, IsfiniteFunction>);
    function_set_ptr->AddFunction(isfinite_function_bfloat16);

    NewCatalog::AddFunctionSet(catalog_ptr, function_set_ptr);
}

} // namespace infinity
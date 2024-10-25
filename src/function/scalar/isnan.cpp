module;

#include <cmath>

module isnan;

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

struct IsnanFunction {
    template <typename TA, typename TB>
    static inline void Run(TA &left, TB &result) {
        Status status = Status::NotSupport("Not implemented");
        RecoverableError(status);
        /*
        if constexpr (std::is_same_v<std::remove_cv_t<TA>, Float16T> || std::is_same_v<std::remove_cv_t<TA>, BFloat16T>) {
            result = std::isnan(static_cast<float>(left));
        } else if constexpr(std::is_same_v<std::remove_cv_t<TA>, FloatT> || std::is_same_v<std::remove_cv_t<TA>, DoubleT>){
            result = std::isnan(left);
        } else {
            Status status = Status::NotSupport("Not implemented");
            RecoverableError(status);
        }
        */
    }
};


template <>
inline void IsnanFunction::Run(FloatT &left, BooleanT &result) {
    result = std::isnan(left);
}

template <>
inline void IsnanFunction::Run(DoubleT &left, BooleanT &result) {
    result = std::isnan(left);
}

template <>
inline void IsnanFunction::Run(Float16T &left, BooleanT &result) {
    result = std::isnan(static_cast<float>(left));
}

template <>
inline void IsnanFunction::Run(BFloat16T &left, BooleanT &result) {
    result = std::isnan(static_cast<float>(left));
}


void RegisterIsnanFunction(const UniquePtr<Catalog> &catalog_ptr){
    String func_name = "isnan";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    ScalarFunction isnan_function_float(func_name,
                                      {DataType(LogicalType::kFloat)},
                                      {DataType(LogicalType::kBoolean)},
                                      &ScalarFunction::UnaryFunction<FloatT, BooleanT, IsnanFunction>);
    function_set_ptr->AddFunction(isnan_function_float);

    ScalarFunction isnan_function_double(func_name,
                                       {DataType(LogicalType::kDouble)},
                                       {DataType(LogicalType::kBoolean)},
                                       &ScalarFunction::UnaryFunction<DoubleT, BooleanT, IsnanFunction>);
    function_set_ptr->AddFunction(isnan_function_double);

    ScalarFunction isnan_function_float16(func_name,
                                        {DataType(LogicalType::kFloat16)},
                                        {DataType(LogicalType::kBoolean)},
                                        &ScalarFunction::UnaryFunction<Float16T, BooleanT, IsnanFunction>);
    function_set_ptr->AddFunction(isnan_function_float16);

    ScalarFunction isnan_function_bfloat16(func_name,
                                         {DataType(LogicalType::kBFloat16)},
                                         {DataType(LogicalType::kBoolean)},
                                         &ScalarFunction::UnaryFunction<BFloat16T, BooleanT, IsnanFunction>);
    function_set_ptr->AddFunction(isnan_function_bfloat16);

    Catalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

}
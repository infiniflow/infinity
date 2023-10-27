//
// Created by JinHai on 2022/9/28.
//

module;

import stl;
import new_catalog;
import infinity_assert;
import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module or_func;

namespace infinity {

struct OrFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result = left or right;
    }
};

static void GenerateOrFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr) {
    String func_name = "OR";
    ScalarFunction or_function(func_name,
                               {DataType(LogicalType::kBoolean), DataType(LogicalType::kBoolean)},
                               {DataType(LogicalType::kBoolean)},
                               &ScalarFunction::BinaryFunction<BooleanT, BooleanT, BooleanT, OrFunction>);
    function_set_ptr->AddFunction(or_function);
}

void RegisterOrFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "OR";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateOrFunction(function_set_ptr);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
//
// Created by JinHai on 2022/9/28.
//

module;

import stl;
import new_catalog;

import infinity_exception;
import scalar_function;
import scalar_function_set;
import parser;
import third_party;

module and_func;

namespace infinity {

struct AndFunction {
    template <typename TA, typename TB, typename TC>
    static inline void Run(TA left, TB right, TC &result) {
        result = left and right;
    }
};

static void GenerateAndFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr) {
    String func_name = "AND";
    ScalarFunction and_function(func_name,
                                {DataType(LogicalType::kBoolean), DataType(LogicalType::kBoolean)},
                                {DataType(LogicalType::kBoolean)},
                                &ScalarFunction::BinaryFunction<BooleanT, BooleanT, BooleanT, AndFunction>);
    function_set_ptr->AddFunction(and_function);
}

void RegisterAndFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "AND";
    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateAndFunction(function_set_ptr);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
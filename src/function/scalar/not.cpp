//
// Created by jinhai on 23-1-12.
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

module not_func;

namespace infinity {

struct NotFunction {
    template <typename TA, typename TB>
    static inline void Run(TA input, TB &result) {
        result = !input;
    }
};

static void GenerateNotFunction(SharedPtr<ScalarFunctionSet> &function_set_ptr) {
    String func_name = "NOT";

    ScalarFunction not_function(func_name,
                                {DataType(LogicalType::kBoolean)},
                                {DataType(LogicalType::kBoolean)},
                                &ScalarFunction::UnaryFunction<BooleanT, BooleanT, NotFunction>);
    function_set_ptr->AddFunction(not_function);
}

void RegisterNotFunction(const UniquePtr<NewCatalog> &catalog_ptr) {
    String func_name = "NOT";

    SharedPtr<ScalarFunctionSet> function_set_ptr = MakeShared<ScalarFunctionSet>(func_name);

    GenerateNotFunction(function_set_ptr);

    NewCatalog::AddFunctionSet(catalog_ptr.get(), function_set_ptr);
}

} // namespace infinity
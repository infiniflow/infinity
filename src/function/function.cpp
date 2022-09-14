//
// Created by JinHai on 2022/9/14.
//

#include "function.h"

namespace infinity {

BuiltinFunctions::BuiltinFunctions(const std::unique_ptr<Catalog> &catalog_ptr) : catalog_ptr_(catalog_ptr) {}

void BuiltinFunctions::Init() {
    RegisterAggregateFunction();
    RegisterScalarFunction();
    RegisterTableFunction();
}

void
BuiltinFunctions::AddFunctionSet(FunctionSet& function_set) {
    // insert the function set into catalog
}

void
BuiltinFunctions::RegisterAggregateFunction() {

}

void
BuiltinFunctions::RegisterScalarFunction() {

}

void
BuiltinFunctions::RegisterTableFunction() {

}

}
//
// Created by JinHai on 2022/9/14.
//

#include "builtin_functions.h"

namespace infinity {

BuiltinFunctions::BuiltinFunctions(std::unique_ptr<Catalog> &catalog_ptr) : catalog_ptr_(catalog_ptr) {}

void BuiltinFunctions::Init() {
    RegisterAggregateFunction();
    RegisterScalarFunction();
    RegisterTableFunction();
}

void
BuiltinFunctions::AddFunctionSet(ScalarFunctionSet& function_set) {
    // insert the function set into catalog
}

void
BuiltinFunctions::AddFunctionSet(TableFunctionSet& function_set) {
    // insert the table function set into catalog
}

void
BuiltinFunctions::AddFunctionSet(AggregateFunctionSet& function_set) {
    // insert the aggregate function set into catalog
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
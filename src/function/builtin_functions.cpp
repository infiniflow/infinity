//
// Created by JinHai on 2022/9/14.
//

#include "builtin_functions.h"
#include "scalar/abs.h"
#include "scalar/pow.h"
#include "scalar/equals.h"
#include "scalar/notequals.h"
#include "scalar/less.h"
#include "scalar/less_equals.h"
#include "scalar/greater.h"
#include "scalar/greater_equals.h"

namespace infinity {

BuiltinFunctions::BuiltinFunctions(std::unique_ptr<Catalog> &catalog_ptr) : catalog_ptr_(catalog_ptr) {}

void
BuiltinFunctions::Init() {
    RegisterAggregateFunction();
    RegisterScalarFunction();
    RegisterTableFunction();
}

void
BuiltinFunctions::RegisterAggregateFunction() {

}

void
BuiltinFunctions::RegisterScalarFunction() {
    // Insert the function set into catalog

    // Math functions
    RegisterAbsFunction(catalog_ptr_);

    RegisterPowFunction(catalog_ptr_);

    // register comparison operator
    RegisterEqualsFunction(catalog_ptr_);
    RegisterNotEqualsFunction(catalog_ptr_);
    RegisterLessFunction(catalog_ptr_);
    RegisterLessEqualsFunction(catalog_ptr_);
    RegisterGreaterFunction(catalog_ptr_);
    RegisterGreaterEqualsFunction(catalog_ptr_);
}

void
BuiltinFunctions::RegisterTableFunction() {

}

}
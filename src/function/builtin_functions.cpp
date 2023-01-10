//
// Created by JinHai on 2022/9/14.
//

#include "builtin_functions.h"

// Arithmetic scalar function
#include "scalar/add.h"

#include "scalar/subtract.h"
#include "scalar/multiply.h"
#include "scalar/divide.h"

// Match scalar function
#include "scalar/abs.h"
#include "scalar/pow.h"

// Match comparison function
#include "scalar/equals.h"
#include "scalar/inequals.h"
#include "scalar/less.h"
#include "scalar/less_equals.h"
#include "scalar/greater.h"
#include "scalar/greater_equals.h"

// Logical scalar function

#include "scalar/or.h"
#include "scalar/and.h"

// Aggregate function

// Table function
#include "table/table_scan.h"

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

    // Arithmetic functions
    RegisterAddFunction(catalog_ptr_);
    RegisterSubtractFunction(catalog_ptr_);
    RegisterMulFunction(catalog_ptr_);
    RegisterDivFunction(catalog_ptr_);

    // Logic functions
    RegisterAndFunction(catalog_ptr_);
    RegisterOrFunction(catalog_ptr_);

    // Math functions
    RegisterAbsFunction(catalog_ptr_);

    RegisterPowFunction(catalog_ptr_);

    // register comparison operator
    RegisterEqualsFunction(catalog_ptr_);
    RegisterInEqualsFunction(catalog_ptr_);
    RegisterLessFunction(catalog_ptr_);
    RegisterLessEqualsFunction(catalog_ptr_);
    RegisterGreaterFunction(catalog_ptr_);
    RegisterGreaterEqualsFunction(catalog_ptr_);
}

void
BuiltinFunctions::RegisterTableFunction() {

    RegisterTableScanFunction(catalog_ptr_);
}

}
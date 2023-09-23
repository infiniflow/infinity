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

#include "function/scalar/like.h"
#include "function/scalar/extract.h"

// Aggregate function
#include "aggregate/avg.h"
#include "aggregate/count.h"
#include "aggregate/first.h"
#include "aggregate/max.h"
#include "aggregate/min.h"
#include "aggregate/sum.h"

// String function
#include "function/scalar/substring.h"

// Table function
#include "table/seq_scan.h"
#include "function/scalar/not.h"
#include "function/table/table_scan.h"
#include "function/table/knn_scan.h"

namespace infinity {

BuiltinFunctions::BuiltinFunctions(UniquePtr<NewCatalog> &catalog_ptr) : catalog_ptr_(catalog_ptr) {}

void
BuiltinFunctions::Init() {
    RegisterAggregateFunction();
    RegisterScalarFunction();
    RegisterTableFunction();
}

void
BuiltinFunctions::RegisterAggregateFunction() {
    RegisterAvgFunction(catalog_ptr_);
    RegisterCountFunction(catalog_ptr_);
    RegisterFirstFunction(catalog_ptr_);
    RegisterMaxFunction(catalog_ptr_);
    RegisterMinFunction(catalog_ptr_);
    RegisterSumFunction(catalog_ptr_);
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
    RegisterNotFunction(catalog_ptr_);

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

    // like function
    RegisterLikeFunction(catalog_ptr_);
    RegisterNotLikeFunction(catalog_ptr_);

    // extract function
    RegisterExtractFunction(catalog_ptr_);

    // string functions
    RegisterSubstringFunction(catalog_ptr_);
}

void
BuiltinFunctions::RegisterTableFunction() {

    RegisterSeqScanFunction(catalog_ptr_);
    RegisterTableScanFunction(catalog_ptr_);
    RegisterKnnScanFunction(catalog_ptr_);
}

}
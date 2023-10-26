//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import new_catalog;
import avg;
import count;
import first;
import max;
import min;
import sum;

import add;
import abs;
import and_func;
import divide;
import equals;
import extract;
import greater;
import greater_equals;
import inequals;
import less;
import less_equals;
import like;
import minus;
import modulo;
import multiply;
import not_func;
import or_func;
import plus;
import pow;
import substring;
import substract;

import table_scan;
import seq_scan;
import knn_scan;

module builtin_functions;

namespace infinity {

BuiltinFunctions::BuiltinFunctions(UniquePtr<NewCatalog> &catalog_ptr) : catalog_ptr_(catalog_ptr) {}

void BuiltinFunctions::Init() {
    RegisterAggregateFunction();
    RegisterScalarFunction();
    RegisterTableFunction();
}

void BuiltinFunctions::RegisterAggregateFunction() {
    RegisterAvgFunction(catalog_ptr_);
    RegisterCountFunction(catalog_ptr_);
    RegisterFirstFunction(catalog_ptr_);
    RegisterMaxFunction(catalog_ptr_);
    RegisterMinFunction(catalog_ptr_);
    RegisterSumFunction(catalog_ptr_);
}

void BuiltinFunctions::RegisterScalarFunction() {
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

void BuiltinFunctions::RegisterTableFunction() {

    RegisterSeqScanFunction(catalog_ptr_);
    RegisterTableScanFunction(catalog_ptr_);
    RegisterKnnScanFunction(catalog_ptr_);
}

} // namespace infinity

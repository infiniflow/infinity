// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;
import new_catalog;
import avg;
import count;
import first;
import max;
import min;
import sum;
import count_star;

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
import knn_scan;

import special_function;

import parser;


module builtin_functions;

namespace infinity {

BuiltinFunctions::BuiltinFunctions(UniquePtr<NewCatalog> &catalog_ptr) : catalog_ptr_(catalog_ptr) {}

void BuiltinFunctions::Init() {
    RegisterAggregateFunction();
    RegisterScalarFunction();
    RegisterTableFunction();
    RegisterSpecialFunction();
}

void BuiltinFunctions::RegisterAggregateFunction() {
    RegisterAvgFunction(catalog_ptr_);
    RegisterCountFunction(catalog_ptr_);
    RegisterCountStarFunction(catalog_ptr_);
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

    RegisterTableScanFunction(catalog_ptr_);
    RegisterKnnScanFunction(catalog_ptr_);
}

void BuiltinFunctions::RegisterSpecialFunction() {

    SharedPtr<SpecialFunction> row_function = MakeShared<SpecialFunction>("ROW_ID", DataType(LogicalType::kBigInt), 1, SpecialType::kRowID);
    NewCatalog::AddSpecialFunction(catalog_ptr_.get(), row_function);

    SharedPtr<SpecialFunction> create_ts_function = MakeShared<SpecialFunction>("DISTANCE", DataType(LogicalType::kFloat), 2, SpecialType::kDistance);
    NewCatalog::AddSpecialFunction(catalog_ptr_.get(), create_ts_function);

    SharedPtr<SpecialFunction> delete_ts_function = MakeShared<SpecialFunction>("SCORE", DataType(LogicalType::kFloat), 3, SpecialType::kScore);
    NewCatalog::AddSpecialFunction(catalog_ptr_.get(), delete_ts_function);

}

} // namespace infinity

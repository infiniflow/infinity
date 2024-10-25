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

module builtin_functions;

import stl;
import catalog;
import avg;
import count;
import first;
import max;
import min;
import sum;

import add;
import abs;
import sqrt;
import round;
import ceil;
import floor;
import log;
import isnan;
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
import char_length;
import md5;
import lower;
import upper;
import regex;
import ltrim;
import rtrim;
import trim;
import position;
import default_values;
import special_function;
import internal_types;
import data_type;

import logical_type;

namespace infinity {

BuiltinFunctions::BuiltinFunctions(UniquePtr<Catalog> &catalog_ptr) : catalog_ptr_(catalog_ptr) {}

void BuiltinFunctions::Init() {
    RegisterAggregateFunction();
    RegisterScalarFunction();
    RegisterTableFunction();
    RegisterSpecialFunction();
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
    AppendRegisterPlusFunction(catalog_ptr_);
    RegisterSubtractFunction(catalog_ptr_);
    AppendRegisterMinusFunction(catalog_ptr_);
    RegisterMulFunction(catalog_ptr_);
    RegisterDivFunction(catalog_ptr_);
    RegisterModuloFunction(catalog_ptr_);

    // Logic functions
    RegisterAndFunction(catalog_ptr_);
    RegisterOrFunction(catalog_ptr_);
    RegisterNotFunction(catalog_ptr_);

    // Math functions
    RegisterAbsFunction(catalog_ptr_);
    RegisterPowFunction(catalog_ptr_);
    RegisterSqrtFunction(catalog_ptr_);
    RegisterRoundFunction(catalog_ptr_);
    RegisterCeilFunction(catalog_ptr_);
    RegisterFloorFunction(catalog_ptr_);
    RegisterLogFunction(catalog_ptr_);
    RegisterIsnanFunction(catalog_ptr_);

    // register comparison operator
    RegisterEqualsFunction(catalog_ptr_);
    RegisterInEqualFunction(catalog_ptr_);
    RegisterInEqualAliasFunction(catalog_ptr_);
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
    RegisterCharLengthFunction(catalog_ptr_);
    RegisterMd5Function(catalog_ptr_);
    RegisterLowerFunction(catalog_ptr_);
    RegisterUpperFunction(catalog_ptr_);
    RegisterRegexFunction(catalog_ptr_);
    RegisterLtrimFunction(catalog_ptr_);
    RegisterRtrimFunction(catalog_ptr_);
    RegisterTrimFunction(catalog_ptr_);
    RegisterPositionFunction(catalog_ptr_);
}

void BuiltinFunctions::RegisterTableFunction() {}

void BuiltinFunctions::RegisterSpecialFunction() {

    SharedPtr<SpecialFunction> row_function = MakeShared<SpecialFunction>("ROW_ID", DataType(LogicalType::kBigInt), 1, SpecialType::kRowID);
    Catalog::AddSpecialFunction(catalog_ptr_.get(), row_function);

    SharedPtr<SpecialFunction> distance_function = MakeShared<SpecialFunction>("DISTANCE", DataType(LogicalType::kFloat), 2, SpecialType::kDistance);
    Catalog::AddSpecialFunction(catalog_ptr_.get(), distance_function);

    SharedPtr<SpecialFunction> similarity_function =
        MakeShared<SpecialFunction>("SIMILARITY", DataType(LogicalType::kFloat), 3, SpecialType::kSimilarity);
    Catalog::AddSpecialFunction(catalog_ptr_.get(), similarity_function);

    SharedPtr<SpecialFunction> score_function = MakeShared<SpecialFunction>("SCORE", DataType(LogicalType::kFloat), 4, SpecialType::kScore);
    Catalog::AddSpecialFunction(catalog_ptr_.get(), score_function);

    auto createts_function =
        MakeShared<SpecialFunction>("CREATE_TIMESTAMP", DataType(LogicalType::kBigInt), COLUMN_IDENTIFIER_CREATE, SpecialType::kCreateTs);
    Catalog::AddSpecialFunction(catalog_ptr_.get(), createts_function);

    auto deletets_function =
        MakeShared<SpecialFunction>("DELETE_TIMESTAMP", DataType(LogicalType::kBigInt), COLUMN_IDENTIFIER_DELETE, SpecialType::kDeleteTs);
    Catalog::AddSpecialFunction(catalog_ptr_.get(), deletets_function);

    Catalog::AddSpecialFunction(catalog_ptr_.get(),
                                MakeShared<SpecialFunction>("FILTER_FULLTEXT", DataType(LogicalType::kBoolean), 0, SpecialType::kFilterFullText));

    Catalog::AddSpecialFunction(catalog_ptr_.get(),
                                MakeShared<SpecialFunction>("FILTER_TEXT", DataType(LogicalType::kBoolean), 0, SpecialType::kFilterFullText));
}

} // namespace infinity

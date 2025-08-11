// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#ifdef CI
#include "gtest/gtest.h"
import infinity_core;
import base_test;
#else
module;

#include "gtest/gtest.h"

module infinity_core:ut.current_timestamp_functions;

import :ut.base_test;
import :stl;
import :infinity_exception;
import :infinity_context;
import :new_catalog;
import :logger;
import :default_values;
import :value;
import :base_expression;
import :column_expression;
import :column_vector;
import :data_block;
import :function_set;
import :function;
import :scalar_function;
import :scalar_function_set;
import :current_timestamp;
import :third_party;
import :config;
import :status;
import :kv_store;
#endif

import global_resource_usage;
import data_type;
import internal_types;
import logical_type;

using namespace infinity;

class CurrentTimestampFunctionsTest : public BaseTest {};

TEST_F(CurrentTimestampFunctionsTest, current_timestamp_func) {
    using namespace infinity;

    UniquePtr<Config> config_ptr = MakeUnique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    UniquePtr<KVStore> kv_store_ptr = MakeUnique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    UniquePtr<NewCatalog> catalog_ptr = MakeUnique<NewCatalog>(kv_store_ptr.get());

    RegisterCurrentTimestampFunction(catalog_ptr.get());

    String op = "current_timestamp";

    SharedPtr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    SharedPtr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<SharedPtr<BaseExpression>> inputs;

        DataType data_type1(LogicalType::kVarchar);
        SharedPtr<DataType> result_type = MakeShared<DataType>(LogicalType::kTimestamp);
        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("current_timestamp()", func.ToString().c_str());

        Vector<SharedPtr<DataType>> column_types;
        column_types.emplace_back(MakeShared<DataType>(data_type1));

        DataBlock data_block;

        SharedPtr<ColumnVector> result = MakeShared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (SizeT i = 0; i < 1; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kTimestamp);
        }
    }
}

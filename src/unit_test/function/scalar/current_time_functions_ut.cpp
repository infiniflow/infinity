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

module infinity_core:ut.current_time_functions;

import :ut.base_test;

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
import :current_time;
import third_party;
import :config;
import :status;
import :kv_store;
#endif

import global_resource_usage;
import data_type;
import internal_types;
import logical_type;

using namespace infinity;

class CurrentTimeFunctionsTest : public BaseTest {};

TEST_F(CurrentTimeFunctionsTest, current_time_func) {
    using namespace infinity;

    std::unique_ptr<Config> config_ptr = std::make_unique<Config>();
    Status status = config_ptr->Init(nullptr, nullptr);
    EXPECT_TRUE(status.ok());
    std::unique_ptr<KVStore> kv_store_ptr = std::make_unique<KVStore>();
    status = kv_store_ptr->Init(config_ptr->CatalogDir());
    EXPECT_TRUE(status.ok());
    std::unique_ptr<NewCatalog> catalog_ptr = std::make_unique<NewCatalog>(kv_store_ptr.get());

    RegisterCurrentTimeFunction(catalog_ptr.get());

    String op = "current_time";

    std::shared_ptr<FunctionSet> function_set = NewCatalog::GetFunctionSetByName(catalog_ptr.get(), op);
    EXPECT_EQ(function_set->type_, FunctionType::kScalar);
    std::shared_ptr<ScalarFunctionSet> scalar_function_set = std::static_pointer_cast<ScalarFunctionSet>(function_set);

    {
        Vector<std::shared_ptr<BaseExpression>> inputs;

        DataType data_type1(LogicalType::kVarchar);
        std::shared_ptr<DataType> result_type = std::make_shared<DataType>(LogicalType::kTime);

        ScalarFunction func = scalar_function_set->GetMostMatchFunction(inputs);
        EXPECT_STREQ("current_time()", func.ToString().c_str());

        Vector<std::shared_ptr<DataType>> column_types;
        column_types.emplace_back(std::make_shared<DataType>(data_type1));

        DataBlock data_block;
        data_block.Init(column_types);

        std::shared_ptr<ColumnVector> result = std::make_shared<ColumnVector>(result_type);
        result->Initialize();
        func.function_(data_block, result);

        for (size_t i = 0; i < 1; ++i) {
            Value v = result->GetValueByIndex(i);
            EXPECT_EQ(v.type_.type(), LogicalType::kTime);
        }
    }
}

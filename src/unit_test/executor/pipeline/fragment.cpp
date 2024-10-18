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

#include "gtest/gtest.h"
import base_test;

import infinity_exception;

import global_resource_usage;
import third_party;

import logger;
import stl;
import infinity_context;
import sql_runner;

using namespace infinity;
class FragmentTest : public BaseTest {
    void SetUp() override {
        BaseTest::SetUp();
        BaseTest::RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTest::TearDown();
    }
};

TEST_F(FragmentTest, test_build_fragment) {
    using namespace infinity;
    /// DDL
    auto result0 = SQLRunner::Run("create table t1(a bigint)", true);
    EXPECT_EQ(result0->definition_ptr_.get()->columns()[0]->name_, "OK");
    auto result1 = SQLRunner::Run("create database db1", true);
    EXPECT_EQ(result1->definition_ptr_.get()->columns()[0]->name_, "OK");
    auto result2 = SQLRunner::Run("create table db1.t1(a bigint)", true);
    EXPECT_EQ(result2->definition_ptr_.get()->columns()[0]->name_, "OK");
    auto result3 = SQLRunner::Run("create table t2(a bigint)", true);
    EXPECT_EQ(result3->definition_ptr_.get()->columns()[0]->name_, "OK");
    auto result4 = SQLRunner::Run("create table t3(c1 embedding(bit,10))", true);
    EXPECT_EQ(result4->definition_ptr_.get()->columns()[0]->name_, "OK");
    auto result5 = SQLRunner::Run("drop database db1", true);
    EXPECT_EQ(result5->definition_ptr_.get()->columns()[0]->name_, "OK");

    /// SPJ
    //    SQLRunner::Run("select * from t1 where a = 1", true);
    //    SQLRunner::Run("select a+1 from t1", true);

    /// DDL
    auto result6 = SQLRunner::Run("drop table t1", true);
    EXPECT_EQ(result6->definition_ptr_.get()->columns()[0]->name_, "OK");

    /// Show
    auto result7 = SQLRunner::Run("show tables", true);
    EXPECT_EQ(result7->definition_ptr_->column_count(), 9u);
    auto result8 = SQLRunner::Run("show table t2", true);
    EXPECT_EQ(result8->definition_ptr_->column_count(), 2u);

    /// DDL
    auto result9 = SQLRunner::Run("drop table t3", true);
    EXPECT_EQ(result9->definition_ptr_.get()->columns()[0]->name_, "OK");
    auto result10 = SQLRunner::Run("drop table t2", true);
    EXPECT_EQ(result9->definition_ptr_.get()->columns()[0]->name_, "OK");
}

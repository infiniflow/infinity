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

#ifdef CI
#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"
import infinity_core;
import base_test;
import sql_runner;
#else
module;

#include "unit_test/gtest_expand.h"
#include "gtest/gtest.h"

module infinity_core:ut.logical_planner;

import :ut.base_test;
import :ut.sql_runner;
import :infinity_exception;
import third_party;
import :logger;
import :stl;
import :infinity_context;
#endif

import global_resource_usage;

#if 0
class LogicalPlannerTest : public BaseTest {};

TEST_F(LogicalPlannerTest, test1) {
    using namespace infinity;

    SQLRunner::Run("create table t1(a bigint, b bigint);", false);
    SQLRunner::Run("insert into t1 values(1, 2);", false);
    SQLRunner::Run("insert into t1 values(2, 4);", false);
    SQLRunner::Run("insert into t1 values(3, 6);", false);
    SQLRunner::Run("create table t2(a bigint, b bigint);", false);
    SQLRunner::Run("insert into t2 values(10, 20);", false);
    SQLRunner::Run("insert into t2 values(20, 40);", false);
    SQLRunner::Run("create table t3(a bigint, b bigint);", false);
    SQLRunner::Run("insert into t3 values(5, 25);", false);
    SQLRunner::Run("insert into t3 values(15, 35);", false);
    SQLRunner::Run("show tables;", false);
#if 1
    {
        const String sql_text = "select * from t1;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select a from t1;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select t1.a from t1;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select b from t1;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select t1.b from t1;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select abs(b) from t1;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select a+1 from t1;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select a+1 from t1 order by b;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select a, b from t1 group by a, b;";
        SQLRunner::Run(sql_text, true);
    }
    {
        const String sql_text = "select sum(b), b from t1 group by a;";
        EXPECT_THROW_WITHOUT_STACKTRACE(SQLRunner::Run(sql_text, true), UnrecoverableException);
    }
    {
        const String sql_text = "select sum(b), b from t1;";
        EXPECT_THROW_WITHOUT_STACKTRACE(SQLRunner::Run(sql_text, true), UnrecoverableException);
    }

    {
        const String sql_text = "select sum(b), count(a) from t1;";
        SQLRunner::Run(sql_text, true);
    }
#endif
    {
        const String sql_text = "select * from t1, t2;";
        SQLRunner::Run(sql_text, true);
    }
#if 1
    {
        const String sql_text = "select * from t1, t2, t3 where t1.a < 3;";
        SQLRunner::Run(sql_text, true);
    }
#endif


}

#endif

//
// Created by jinhai on 23-1-17.
//
#if 0
#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity;
import sql_runner;

class LogicalPlannerTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        system("rm -rf /tmp/infinity");
    }
};

TEST_F(LogicalPlannerTest, test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

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
        EXPECT_THROW(SQLRunner::Run(sql_text, true), PlannerException);
    }
    {
        const String sql_text = "select sum(b), b from t1;";
        EXPECT_THROW(SQLRunner::Run(sql_text, true), PlannerException);
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
//    {
//        // Only check the ast.
//        const String sql_text = "(SELECT * FROM students INTERSECT SELECT * FROM students_2) UNION SELECT * FROM students_3 ORDER BY grade ASC;";
//        SQLRunner::Run(sql_text, true);
//    }

//    SQLRunner::Run("WITH cte AS (SELECT 42 AS i), cte2 AS (SELECT i*100 AS x FROM cte) SELECT * FROM cte2;");

//    BuiltinFunctions builtin_functions(catalog_ptr);
//    builtin_functions.Init();
}
#endif
//
// Created by xwg on 23-9-6.
//
#include <gtest/gtest.h>
#include "test_helper/sql_runner.h"
#include "base_test.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "function/scalar/subtract.h"
#include "expression/column_expression.h"



class FragmentTest : public BaseTest {
    void
    SetUp() override {
        system("rm -rf /tmp/infinity/");
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
        system("rm -rf /tmp/infinity/");
    }
};

TEST_F(FragmentTest, test_build_fragment) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    /// DDL
    SQLRunner::RunV2("create table t1(a bigint)", true);
    SQLRunner::RunV2("create schema s1", true);
    SQLRunner::RunV2("create table s1.t1(a bigint)", true);
    SQLRunner::RunV2("create table t2(a bigint)", true);
    SQLRunner::RunV2("create table t3(c1 embedding(bit,10))", true);
    SQLRunner::RunV2("drop schema s1", true);
    SQLRunner::RunV2("drop table t1", true);

    /// Show
    SQLRunner::RunV2("show tables", true);
    SQLRunner::RunV2("describe t2", true);

    /// SPJ
//    SQLRunner::RunV2("select * from t1 where a = 1", true);
//    SQLRunner::RunV2("select a+1 from t1", true);

}


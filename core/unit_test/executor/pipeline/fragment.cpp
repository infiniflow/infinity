//
// Created by xwg on 23-9-6.
//

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

class FragmentTest : public BaseTest {
    void SetUp() override {
        system("rm -rf /tmp/infinity/");
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        system("rm -rf /tmp/infinity/");
    }
};

TEST_F(FragmentTest, test_build_fragment) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
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
    EXPECT_EQ(result7->definition_ptr_->column_count(), 8);
    auto result8 = SQLRunner::Run("describe t2", true);
    EXPECT_EQ(result8->definition_ptr_->column_count(), 3);
}

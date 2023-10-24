//
// Created by jinhai on 23-3-10.
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
import new_catalog;
import parser;
import function_set;
import aggregate_function_set;
import aggregate_function;
import function;
import column_expression;
import value;
import default_values;
import data_block;
import varchar_cast;

class VarcharTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(VarcharTest, varchar_cast0) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    {

        DataType source_type(LogicalType::kVarchar);
        String s1 = "true";

        {
            DataType target_type(LogicalType::kBoolean);

            VarcharT v1;
            v1.Initialize(s1);
            EXPECT_EQ(v1.length, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{false};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(target);
            EXPECT_TRUE(result);
        }

        s1 = "false";
        {
            VarcharT v1;
            v1.Initialize(s1);
            EXPECT_EQ(v1.length, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{true};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(target);
            EXPECT_TRUE(result);
        }

        s1 = "falsk";
        {
            VarcharT v1;
            v1.Initialize(s1);
            EXPECT_EQ(v1.length, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            BooleanT target{true};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(result);
        }
    }

    {

        DataType source_type(LogicalType::kVarchar);
        String s1 = "-128";

        {
            VarcharT v1;
            v1.Initialize(s1);
            EXPECT_EQ(v1.length, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(result);
            EXPECT_EQ(target, -128);
        }

        s1 = "127";
        {
            DataType target_type(LogicalType::kBoolean);

            VarcharT v1;
            v1.Initialize(s1);
            EXPECT_EQ(v1.length, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(result);
            EXPECT_EQ(target, 127);
        }

        s1 = "190";
        {
            DataType target_type(LogicalType::kBoolean);

            VarcharT v1;
            v1.Initialize(s1);
            EXPECT_EQ(v1.length, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_TRUE(result);
        }

        s1 = "abc";
        {
            DataType target_type(LogicalType::kBoolean);

            VarcharT v1;
            v1.Initialize(s1);
            EXPECT_EQ(v1.length, s1.length());
            EXPECT_EQ(v1.ToString(), s1);

            TinyIntT target{0};
            bool result = TryCastVarchar::Run(v1, target);
            EXPECT_FALSE(result);
            EXPECT_EQ(target, 0);
        }
    }
}
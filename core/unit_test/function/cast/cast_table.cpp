//
// Created by jinhai on 23-1-5.
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
import cast_table;
import column_vector;

class CastTableTest : public BaseTest {
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

TEST_F(CastTableTest, casttable_boolean) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    for (i8 to = LogicalType::kBoolean; to < LogicalType::kInvalid; ++to) {
        switch (to) {
            case LogicalType::kBoolean: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, LogicalType::kBoolean), 0);
                break;
            }
            case LogicalType::kVarchar: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, LogicalType::kVarchar), 1);
                break;
            }
            default: {
                EXPECT_EQ(CastTable::instance().GetCastCost(LogicalType::kBoolean, static_cast<LogicalType>(to)), -1);
            }
        }
    }
}
//
// Created by JinHai on 2022/11/15.
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

class BoxTypeTest : public BaseTest {
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

TEST_F(BoxTypeTest, point1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    BoxT b1;
    b1.upper_left.x = 1.0f;
    b1.upper_left.y = 2.0f;
    b1.lower_right.x = 3.0f;
    b1.lower_right.y = 4.0f;

    BoxT b2(b1);

    EXPECT_EQ(b2.upper_left.x, 1.0f);
    EXPECT_EQ(b2.upper_left.y, 2.0f);
    EXPECT_EQ(b2.lower_right.x, 3.0f);
    EXPECT_EQ(b2.lower_right.y, 4.0f);

    EXPECT_EQ(b1, b2);

    b1.Reset();

    EXPECT_EQ(b1.upper_left.x, 0);
    EXPECT_EQ(b1.upper_left.y, 0);
    EXPECT_EQ(b1.lower_right.x, 0);
    EXPECT_EQ(b1.lower_right.x, 0);
}
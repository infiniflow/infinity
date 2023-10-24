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

class PointTypeTest : public BaseTest {
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

TEST_F(PointTypeTest, point1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    PointT p1;
    p1.x = 1.0f;
    p1.y = 2.0f;

    PointT p2(1.0f, 2.0f);

    EXPECT_EQ(p1.x, 1.0f);
    EXPECT_EQ(p1.y, 2.0f);
    EXPECT_EQ(p2.x, 1.0f);
    EXPECT_EQ(p2.y, 2.0f);

    EXPECT_EQ(p1, p2);

    p2.Reset();

    EXPECT_EQ(p2.x, 0);
    EXPECT_EQ(p2.y, 0);
}

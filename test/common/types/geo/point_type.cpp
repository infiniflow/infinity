//
// Created by JinHai on 2022/11/15.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class PointTypeTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(PointTypeTest, point1) {
    using namespace infinity;

    PointT p1;
    p1.x = 1.0f;
    p1.y = 2.0f;

    PointT p2(1.0f, 2.0f);

    EXPECT_EQ(p1.x, 1.0f);
    EXPECT_EQ(p1.y, 2.0f);
    EXPECT_EQ(p2.x, 1.0f);
    EXPECT_EQ(p2.y, 2.0f);

    p2.Reset();

    EXPECT_EQ(p2.x, 0);
    EXPECT_EQ(p2.y, 0);
}

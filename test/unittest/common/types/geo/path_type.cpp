//
// Created by JinHai on 2022/11/14.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class PathTypeTest : public BaseTest {
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
    }
};

TEST_F(PathTypeTest, path1) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    PathT path1;
    EXPECT_EQ(path1.PointCount(), 0);
    EXPECT_THROW(path1.GetPoint(0), TypeException);
    EXPECT_THROW(path1.SetPoint(0, PointT(1.0, 2.1)), TypeException);


    path1.Initialize(2);
    EXPECT_THROW(path1.Initialize(0), TypeException);
    path1.SetPoint(0, PointT(1.0, 2.1));
    path1.SetPoint(1, PointT(1.1, 2.2));
    EXPECT_THROW(path1.SetPoint(2, PointT(1.3, 2.5)), TypeException);
    EXPECT_EQ(path1.PointCount(), 2);

    EXPECT_EQ(path1.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path1.GetPoint(0).y, PointT(1.0, 2.1).y);
//    path1.SetPointCount(2);
    EXPECT_EQ(path1.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path1.GetPoint(1).y, PointT(1.1, 2.2).y);
    path1.Reset();

    EXPECT_EQ(path1.PointCount(), 0);

    EXPECT_THROW(path1.SetPoint(0, PointT(1.0, 2.1)), TypeException);

    PathT path2(2);
    path2.SetPoint(0, PointT(1.0, 2.1));
    path2.SetPoint(1, PointT(1.1, 2.2));
    EXPECT_EQ(path2.PointCount(), 2);

    PathT path3(3);
    EXPECT_EQ(path3.PointCount(), 3);
    path3 = path2;
    EXPECT_EQ(path3.PointCount(), 2);
    EXPECT_EQ(path3.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path3.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path3.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path3.GetPoint(1).y, PointT(1.1, 2.2).y);

    PathT path4(2);
    path4 = path2;
    EXPECT_EQ(path4.PointCount(), 2);
    EXPECT_EQ(path4.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path4.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path4.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path4.GetPoint(1).y, PointT(1.1, 2.2).y);

    PathT path5(1);
    path5 = std::move(path2);
    EXPECT_EQ(path5.PointCount(), 2);
    EXPECT_EQ(path5.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path5.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path5.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path5.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(path2.PointCount(), 0);
    EXPECT_EQ(path2.ptr, nullptr);

    PathT path6(std::move(path4));
    EXPECT_EQ(path6.PointCount(), 2);
    EXPECT_EQ(path6.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path6.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path6.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path6.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(path4.PointCount(), 0);
    EXPECT_EQ(path4.ptr, nullptr);

    PathT path7(path5);
    EXPECT_EQ(path7.PointCount(), 2);
    EXPECT_EQ(path7.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(path7.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(path7.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(path7.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(path5.PointCount(), 2);
    EXPECT_NE(path5.ptr, nullptr);

    EXPECT_EQ(path6, path5);
}

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

class PolygonTypeTest : public BaseTest {
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

TEST_F(PolygonTypeTest, polygon1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    PolygonT polygon1;
    EXPECT_EQ(polygon1.PointCount(), 0);
    EXPECT_THROW(polygon1.GetPoint(0), ParserException);
    EXPECT_THROW(polygon1.SetPoint(0, PointT(1.0, 2.1)), ParserException);

    polygon1.Initialize(2);
    EXPECT_THROW(polygon1.Initialize(0), ParserException);
    polygon1.SetPoint(0, PointT(1.0, 2.1));
    polygon1.SetPoint(1, PointT(1.1, 2.2));
    EXPECT_THROW(polygon1.SetPoint(2, PointT(1.3, 2.5)), ParserException);
    EXPECT_EQ(polygon1.PointCount(), 2);
    EXPECT_EQ(polygon1.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon1.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon1.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon1.bounding_box.lower_right.y, 2.1);

    EXPECT_EQ(polygon1.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon1.GetPoint(0).y, PointT(1.0, 2.1).y);
    //    path1.SetPointCount(2);
    EXPECT_EQ(polygon1.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon1.GetPoint(1).y, PointT(1.1, 2.2).y);
    polygon1.Reset();

    EXPECT_EQ(polygon1.PointCount(), 0);

    EXPECT_THROW(polygon1.SetPoint(0, PointT(1.0, 2.1)), ParserException);

    PolygonT polygon2(2);
    polygon2.SetPoint(0, PointT(1.0, 2.1));
    polygon2.SetPoint(1, PointT(1.1, 2.2));
    EXPECT_EQ(polygon2.PointCount(), 2);

    PolygonT polygon3(3);
    EXPECT_EQ(polygon3.PointCount(), 3);
    EXPECT_EQ(polygon1.bounding_box.upper_left.x, std::numeric_limits<f64>::max());
    EXPECT_EQ(polygon1.bounding_box.upper_left.y, -std::numeric_limits<f64>::max());
    EXPECT_EQ(polygon1.bounding_box.lower_right.x, -std::numeric_limits<f64>::max());
    EXPECT_EQ(polygon1.bounding_box.lower_right.y, std::numeric_limits<f64>::max());

    polygon3 = polygon2;
    EXPECT_EQ(polygon3.PointCount(), 2);
    EXPECT_EQ(polygon3.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon3.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon3.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon3.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon3.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon3.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon3.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon3.bounding_box.lower_right.y, 2.1);

    PolygonT polygon4(2);
    polygon4 = polygon2;
    EXPECT_EQ(polygon4.PointCount(), 2);
    EXPECT_EQ(polygon4.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon4.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon4.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon4.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon4.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon4.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon4.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon4.bounding_box.lower_right.y, 2.1);

    PolygonT polygon5(1);
    polygon5 = std::move(polygon2);
    EXPECT_EQ(polygon5.PointCount(), 2);
    EXPECT_EQ(polygon5.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon5.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon5.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon5.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon5.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon5.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon5.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon5.bounding_box.lower_right.y, 2.1);

    EXPECT_EQ(polygon2.PointCount(), 0);
    EXPECT_EQ(polygon2.ptr, nullptr);

    PolygonT polygon6(std::move(polygon4));
    EXPECT_EQ(polygon6.PointCount(), 2);
    EXPECT_EQ(polygon6.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon6.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon6.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon6.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon4.PointCount(), 0);
    EXPECT_EQ(polygon4.ptr, nullptr);

    EXPECT_EQ(polygon6.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon6.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon6.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon6.bounding_box.lower_right.y, 2.1);

    PolygonT polygon7(polygon5);
    EXPECT_EQ(polygon7.PointCount(), 2);
    EXPECT_EQ(polygon7.GetPoint(0).x, PointT(1.0, 2.1).x);
    EXPECT_EQ(polygon7.GetPoint(0).y, PointT(1.0, 2.1).y);
    EXPECT_EQ(polygon7.GetPoint(1).x, PointT(1.1, 2.2).x);
    EXPECT_EQ(polygon7.GetPoint(1).y, PointT(1.1, 2.2).y);

    EXPECT_EQ(polygon7.bounding_box.upper_left.x, 1.0);
    EXPECT_EQ(polygon7.bounding_box.upper_left.y, 2.2);
    EXPECT_EQ(polygon7.bounding_box.lower_right.x, 1.1);
    EXPECT_EQ(polygon7.bounding_box.lower_right.y, 2.1);

    EXPECT_EQ(polygon5.PointCount(), 2);
    EXPECT_NE(polygon5.ptr, nullptr);

    EXPECT_EQ(polygon6 == polygon7, true);
}
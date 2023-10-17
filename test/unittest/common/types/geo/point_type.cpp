//
// Created by JinHai on 2022/11/15.
//

#include "base_test.h"
#include "main/infinity.h"

class PointTypeTest : public BaseTest {};

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

    EXPECT_EQ(p1, p2);

    p2.Reset();

    EXPECT_EQ(p2.x, 0);
    EXPECT_EQ(p2.y, 0);
}

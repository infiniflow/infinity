//
// Created by jinhai on 23-2-17.
//

#include "base_test.h"
#include "main/infinity.h"
#include "planner/column_binding.h"

class ColumnBindingTest : public BaseTest {};

TEST_F(ColumnBindingTest, test1) {
    using namespace infinity;

    ColumnBinding column1(0, 1);
    ColumnBinding column2(0, 1);
    ColumnBinding column3(1, 2);

    EXPECT_TRUE(column1 == column2);
    EXPECT_TRUE(column1 != column3);
}
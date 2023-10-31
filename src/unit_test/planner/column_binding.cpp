//
// Created by jinhai on 23-2-17.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import global_resource_usage;
import third_party;
import parser;
import logger;
import stl;
import infinity_context;
import column_binding;

class ColumnBindingTest : public BaseTest {};

TEST_F(ColumnBindingTest, test1) {
    using namespace infinity;

    ColumnBinding column1(0, 1);
    ColumnBinding column2(0, 1);
    ColumnBinding column3(1, 2);

    EXPECT_TRUE(column1 == column2);
    EXPECT_TRUE(column1 != column3);
}

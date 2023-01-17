//
// Created by JinHai on 2022/11/28.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class ColumnVectorNestedTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        infinity::Infinity::instance().Init();
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(ColumnVectorNestedTest, flat_array) {
    using namespace infinity;

    // No test cases.
}

TEST_F(ColumnVectorNestedTest, contant_array) {
    using namespace infinity;

    // No test cases.
}

TEST_F(ColumnVectorNestedTest, flat_tuple) {
    using namespace infinity;

    // No test cases.
}

TEST_F(ColumnVectorNestedTest, contant_tuple) {
    using namespace infinity;

    // No test cases.
}


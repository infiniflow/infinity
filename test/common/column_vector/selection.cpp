//
// Created by jinhai on 23-1-24.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/column_vector.h"
#include "common/column_vector/selection.h"
#include "common/types/value.h"
#include "common/default_values.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class SelectionTest : public BaseTest {
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

TEST_F(SelectionTest, test1) {
    using namespace infinity;

    Selection s1;
    EXPECT_THROW(s1.Append(1), ExecutorException);
    EXPECT_THROW(s1.Size() == 0, ExecutorException);
    EXPECT_THROW(s1.Capacity() == 0, ExecutorException);

    EXPECT_THROW(s1.Initialize(std::numeric_limits<u16>::max() + 1), ExecutorException);

    s1.Initialize();
    EXPECT_EQ(s1.Size(), 0);
    EXPECT_EQ(s1.Capacity(), DEFAULT_VECTOR_SIZE);
}
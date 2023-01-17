//
// Created by jinhai on 22-12-19.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/bitmask_buffer.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class BitmaskBufferTest : public BaseTest {
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

TEST_F(BitmaskBufferTest, bitmask_buffer_a) {
    using namespace infinity;

    constexpr size_t bit_count = 8192;

    {
        BitmaskBuffer bitmask_buffer;
        EXPECT_EQ(bitmask_buffer.count_, 0);
        EXPECT_EQ(bitmask_buffer.data_ptr_, nullptr);

        EXPECT_THROW(bitmask_buffer.Initialize(7), TypeException);
        bitmask_buffer.Initialize(bit_count);
        EXPECT_EQ(bitmask_buffer.count_, bit_count);
        EXPECT_NE(bitmask_buffer.data_ptr_, nullptr);

        BitmaskBuffer bitmask_buffer1;
        EXPECT_EQ(bitmask_buffer1.count_, 0);
        EXPECT_EQ(bitmask_buffer1.data_ptr_, nullptr);
        bitmask_buffer1.Initialize(bitmask_buffer.data_ptr_.get(), bit_count);
        EXPECT_EQ(bitmask_buffer1.count_, bit_count);
        EXPECT_NE(bitmask_buffer1.data_ptr_, nullptr);

        bitmask_buffer.Initialize(2 * bit_count);
        EXPECT_EQ(bitmask_buffer.count_, 2 * bit_count);
        EXPECT_NE(bitmask_buffer.data_ptr_, nullptr);
    }

    {
        auto bitmask_buffer_ptr = BitmaskBuffer::Make(bit_count);
        EXPECT_EQ(bitmask_buffer_ptr->count_, bit_count);
    }
}
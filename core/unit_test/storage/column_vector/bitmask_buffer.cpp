//
// Created by jinhai on 22-12-19.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import logger;
import bitmask_buffer;
import bitmask;
import third_party;
import stl;

import infinity;
import global_resource_usage;

class BitmaskBufferTest : public BaseTest {
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

TEST_F(BitmaskBufferTest, bitmask_buffer_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

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

TEST_F(BitmaskBufferTest, ReadWrite) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    constexpr size_t bit_count = 8192;
    std::vector<SharedPtr<Bitmask>> masks = {Bitmask::Make(bit_count), Bitmask::Make(bit_count), Bitmask::Make(bit_count)};
    masks[1]->SetAllFalse();
    masks[2]->SetFalse(4096);

    for (int i = 0; i < 3; i++) {
        auto &bitmask = masks[i];
        int32_t exp_size = bitmask->GetSizeInBytes();
        std::vector<char> buf(exp_size);
        char *ptr = buf.data();

        bitmask->WriteAdv(ptr);
        EXPECT_EQ(ptr - buf.data(), exp_size);

        ptr = buf.data();
        auto bitmask2 = Bitmask::ReadAdv(ptr, exp_size);
        EXPECT_EQ(ptr - buf.data(), exp_size);
        EXPECT_NE(bitmask2, nullptr);
        EXPECT_EQ(*bitmask, *bitmask2);
    }
}

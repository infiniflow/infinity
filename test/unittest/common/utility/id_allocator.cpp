//
// Created by jinhai on 23-5-19.
//
#include "common/utility/id_allocator.h"
#include "base_test.h"
#include "common/types/alias/primitives.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include <gtest/gtest.h>

class IDAllocatorTest : public BaseTest {
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

TEST_F(IDAllocatorTest, test1) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());
    IDAllocator id_allocator;
    Vector<Thread> threads;

    constexpr i64 thread_count = 100;

    for (i64 i = 0; i < thread_count; ++i) {
        threads.emplace_back([&] { id_allocator.AllocateNewId(); });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    EXPECT_EQ(id_allocator.GetCurrentID(), thread_count);
}

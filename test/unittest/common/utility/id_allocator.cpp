//
// Created by jinhai on 23-5-19.
//

#include "base_test.h"
#include "main/infinity.h"
#include "common/utility/id_allocator.h"

class IDAllocatorTest : public BaseTest {};

TEST_F(IDAllocatorTest, test1) {
    using namespace infinity;
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

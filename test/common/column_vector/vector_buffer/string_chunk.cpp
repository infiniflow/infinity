//
// Created by JinHai on 2022/11/30.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "common/column_vector/vector_buffer/string_chunk.h"
#include "common/types/value.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"

class StringChunkTest : public BaseTest {
    void
    SetUp() override {
        infinity::Logger::Initialize();
        infinity::GlobalResourceUsage::Init();
    }

    void
    TearDown() override {
        infinity::Logger::Shutdown();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
};

TEST_F(StringChunkTest, string_chunk_a) {
    using namespace infinity;

    StringChunkMgr string_chunk_mgr;

    EXPECT_THROW(string_chunk_mgr.Allocate(0), ExecutorException);

    for(u64 i = 256; i <= 8192; i += 256) {
        string_chunk_mgr.Allocate(i);
    }

    LOG_TRACE("{}", string_chunk_mgr.Stats());
    EXPECT_EQ(string_chunk_mgr.current_chunk_idx(), 26);
    EXPECT_EQ(string_chunk_mgr.current_chunk_size(), 8192);
}

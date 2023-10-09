//
// Created by JinHai on 2022/11/30.
//

#include "base_test.h"
#include "common/column_vector/vector_buffer/heap_chunk.h"
#include "common/types/value.h"
#include "main/infinity.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include <gtest/gtest.h>

class StringChunkTest : public BaseTest {
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

TEST_F(StringChunkTest, string_chunk_a) {
    using namespace infinity;
    LOG_TRACE("Test name: {}.{}", test_info_->test_case_name(), test_info_->name());

    StringHeapMgr string_chunk_mgr;

    EXPECT_THROW(string_chunk_mgr.Allocate(0), ExecutorException);

    for (u64 i = 256; i <= 8192; i += 256) {
        string_chunk_mgr.Allocate(i);
    }

    LOG_TRACE("{}", string_chunk_mgr.Stats());
    EXPECT_EQ(string_chunk_mgr.current_chunk_idx(), 26);
    EXPECT_EQ(string_chunk_mgr.current_chunk_size(), 8192);
}

//
// Created by JinHai on 2022/11/30.
//

#include "base_test.h"
#include "common/column_vector/vector_buffer/heap_chunk.h"
#include "main/infinity.h"

class StringChunkTest : public BaseTest {};

TEST_F(StringChunkTest, string_chunk_a) {
    using namespace infinity;

    StringHeapMgr string_chunk_mgr;

    EXPECT_THROW(string_chunk_mgr.Allocate(0), ExecutorException);

    for (u64 i = 256; i <= 8192; i += 256) {
        string_chunk_mgr.Allocate(i);
    }

    std::cout << string_chunk_mgr.Stats() << std::endl;
    EXPECT_EQ(string_chunk_mgr.current_chunk_idx(), 26);
    EXPECT_EQ(string_chunk_mgr.current_chunk_size(), 8192);
}

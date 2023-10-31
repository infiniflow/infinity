//
// Created by JinHai on 2022/11/30.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import logger;
import heap_chunk;
import third_party;
import stl;
import global_resource_usage;
import infinity_context;

class StringChunkTest : public BaseTest {};

TEST_F(StringChunkTest, string_chunk_a) {
    using namespace infinity;

    StringHeapMgr string_chunk_mgr;

    EXPECT_THROW(string_chunk_mgr.Allocate(0), ExecutorException);

    for (u64 i = 256; i <= 8192; i += 256) {
        string_chunk_mgr.Allocate(i);
    }

    EXPECT_EQ(string_chunk_mgr.current_chunk_idx(), 26);
    EXPECT_EQ(string_chunk_mgr.current_chunk_size(), 8192);
}

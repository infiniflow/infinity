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

class StringChunkTest : public BaseTest {
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(StringChunkTest, string_chunk_a) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

    StringHeapMgr string_chunk_mgr;

    EXPECT_THROW(string_chunk_mgr.Allocate(0), ExecutorException);

    for (u64 i = 256; i <= 8192; i += 256) {
        string_chunk_mgr.Allocate(i);
    }

    LOG_TRACE(Format("{}", string_chunk_mgr.Stats()));
    EXPECT_EQ(string_chunk_mgr.current_chunk_idx(), 26);
    EXPECT_EQ(string_chunk_mgr.current_chunk_size(), 8192);
}

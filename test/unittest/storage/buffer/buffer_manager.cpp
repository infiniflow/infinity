//
// Created by jinhai on 23-7-10.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "storage/buffer/buffer_manager.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

class BufferMgrTest : public BaseTest {
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

TEST_F(BufferMgrTest, test1) {
    using namespace infinity;
    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    String temp_path = "/tmp/infinity/tmp";
    BufferManager buffer_mgr(memory_limit, temp_path);

    EXPECT_EQ(buffer_mgr.mem_limit_, memory_limit);
    EXPECT_EQ(buffer_mgr.Free(1024), nullptr);

    // file: /tmp/infinity/data/c1.col
    String obj_id = "/tmp/infinity/data/c1.col";
    BufferHandle* buf_handle = buffer_mgr.GetBufferHandle(obj_id, BufferType::kFile);
    EXPECT_EQ(buf_handle->GetID(), 1);
    EXPECT_EQ(buf_handle->GetPath(), obj_id);

    ptr_t data_ptr = buf_handle->LoadData();
    EXPECT_NE(data_ptr, nullptr);
    buf_handle->UnloadData();

    ptr_t data_ptr2 = buf_handle->LoadData();
    EXPECT_EQ(data_ptr, data_ptr2);
    buf_handle->UnloadData();

    EXPECT_FALSE(buf_handle->IsFree());

    EXPECT_EQ(buffer_mgr.current_memory_size_, 128);
    EXPECT_EQ(buffer_mgr.Free(memory_limit - 1), nullptr);
    EXPECT_EQ(buffer_mgr.current_memory_size_, 0);

    EXPECT_TRUE(buf_handle->IsFree());

    ptr_t data_ptr3 = buf_handle->LoadData();
    EXPECT_NE(data_ptr3, nullptr);
}
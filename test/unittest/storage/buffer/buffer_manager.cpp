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
    String temp_path = "/tmp/infinity/_tmp";
    String base_path = "/tmp/infinity/data";
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    EXPECT_EQ(buffer_mgr.mem_limit_, memory_limit);
    EXPECT_EQ(buffer_mgr.Free(1024), nullptr);

    // file: /tmp/infinity/data/c1.col
    String obj_id = "c1.col";
    BufferHandle* buf_handle = buffer_mgr.GetBufferHandle("", obj_id, BufferType::kFile);
    EXPECT_EQ(buf_handle->GetID(), 1);
    EXPECT_EQ(buf_handle->GetFilename(), "/tmp/infinity/data/c1.col");

    ptr_t data_ptr = buf_handle->LoadData();
    EXPECT_NE(data_ptr, nullptr);
    buf_handle->UnloadData();

    ptr_t data_ptr2 = buf_handle->LoadData();
    EXPECT_EQ(data_ptr, data_ptr2);
    buf_handle->UnloadData();

    EXPECT_FALSE(buf_handle->IsFree());

    EXPECT_EQ(buffer_mgr.current_memory_size_, 32768);
    EXPECT_EQ(buffer_mgr.Free(memory_limit - 1), nullptr);
    EXPECT_EQ(buffer_mgr.current_memory_size_, 0);

    EXPECT_TRUE(buf_handle->IsFree());

    ptr_t data_ptr3 = buf_handle->LoadData();
    EXPECT_NE(data_ptr3, nullptr);
    buf_handle->UnloadData();
    EXPECT_EQ(buffer_mgr.current_memory_size_, 32768);

    // temp buffer
    String tmp_buf_name = "t1.col.tmp";
    BufferHandle* tmp_buf = buffer_mgr.AllocateBufferHandle("", tmp_buf_name, 1024);
    EXPECT_EQ(tmp_buf->GetID(), 2);
    EXPECT_EQ(tmp_buf->GetFilename(), "/tmp/infinity/_tmp/t1.col.tmp");
    ptr_t tmp_ptr = tmp_buf->LoadData();
    EXPECT_NE(tmp_ptr, nullptr);

    BufferHandle* tmp_buf1 = buffer_mgr.GetBufferHandle("", tmp_buf_name, BufferType::kTempFile);
    EXPECT_EQ(tmp_buf, tmp_buf1);
    ptr_t tmp1_ptr = tmp_buf1->LoadData();
    EXPECT_EQ(tmp_ptr, tmp1_ptr);

    EXPECT_EQ(buffer_mgr.current_memory_size_, 33792);
    EXPECT_NE(buffer_mgr.Free(memory_limit - 1), nullptr);
    EXPECT_EQ(buffer_mgr.current_memory_size_, 1024);

    tmp_buf->UnloadData();
    EXPECT_NE(buffer_mgr.Free(memory_limit - 1), nullptr);
    tmp_buf1->UnloadData();
    EXPECT_EQ(buffer_mgr.Free(memory_limit - 1), nullptr);
    EXPECT_EQ(buffer_mgr.current_memory_size_, 0);

    tmp_ptr = tmp_buf->LoadData();
    EXPECT_EQ(buffer_mgr.current_memory_size_, 1024);
    tmp_buf1->UnloadData();
    EXPECT_EQ(buffer_mgr.Free(memory_limit - 1), nullptr);
    EXPECT_EQ(buffer_mgr.current_memory_size_, 0);
}
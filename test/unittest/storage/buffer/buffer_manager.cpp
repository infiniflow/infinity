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
#include "storage/buffer/buffer_task.h"

class BufferMgrTest : public BaseTest {
    void
    SetUp() override;

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        system("rm -rf /tmp/infinity");
    }
};

void
BufferMgrTest::SetUp() {
    infinity::GlobalResourceUsage::Init();
    infinity::Infinity::instance().Init();

    using namespace infinity;

    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    {
        BufferManager buffer_mgr(memory_limit, base_path, temp_path);
        buffer_mgr.Init();

        constexpr SizeT elem_count = 4096;

        // file: /tmp/infinity/data/c1.col
        SharedPtr<String> obj_id1 = MakeShared<String>("c1.col");
        BufferHandle *buf_handle1 = buffer_mgr.AllocateBufferHandle(nullptr, obj_id1, elem_count * sizeof(i64));
        EXPECT_EQ(buf_handle1->GetID(), 1);
        EXPECT_EQ(buf_handle1->GetFilename(), "/tmp/infinity/_tmp/c1.col");

        ptr_t buf_ptr1 = buf_handle1->LoadData();
        for (i64 i = 0; i < elem_count; ++i) {
            ((i64 *) buf_ptr1)[i] = i;
        }
        SharedPtr<BufferWriteTask> write_task1 = MakeShared<BufferWriteTask>(buf_handle1);

        // file: /tmp/infinity/data/c2.col
        SharedPtr<String> obj_id2 = MakeShared<String>("c2.col");
        BufferHandle *buf_handle2 = buffer_mgr.AllocateBufferHandle(nullptr, obj_id2, elem_count * sizeof(i32));
        EXPECT_EQ(buf_handle2->GetID(), 2);
        EXPECT_EQ(buf_handle2->GetFilename(), "/tmp/infinity/_tmp/c2.col");

        ptr_t buf_ptr2 = buf_handle2->LoadData();
        for (i32 i = 0; i < elem_count; ++i) {
            ((i32 *) buf_ptr2)[i] = i + 10;
        }
        SharedPtr<BufferWriteTask> write_task2 = MakeShared<BufferWriteTask>(buf_handle2);

        AsyncBatchProcessor processor(DEFAULT_READER_PREPARE_QUEUE_SIZE, DEFAULT_READER_COMMIT_QUEUE_SIZE,
                                      BufferIO::OnPrepare, BufferIO::OnCommit);
        processor.Start();

        processor.Submit(write_task1);
        processor.Submit(write_task2);
        write_task1->Wait();
        if (write_task1->IsError()) {
            LOG_ERROR(write_task1->GetError());
        }

        write_task2->Wait();
        if (write_task2->IsError()) {
            LOG_ERROR(write_task2->GetError());
        }
        processor.Stop();
    }
}

TEST_F(BufferMgrTest, test1) {
    using namespace infinity;

    SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
    SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
    SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
    BufferManager buffer_mgr(memory_limit, base_path, temp_path);

    EXPECT_EQ(buffer_mgr.mem_limit_, memory_limit);
    EXPECT_EQ(buffer_mgr.Free(1024), nullptr);

    SharedPtr<String> tmp_base_name = MakeShared<String>("");

    // file: /tmp/infinity/data/c1.col
    SharedPtr<String> obj_id = MakeShared<String>("c1.col");
    BufferHandle* buf_handle = buffer_mgr.GetBufferHandle(tmp_base_name, obj_id, BufferType::kFile);
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
    SharedPtr<String> tmp_buf_name = MakeShared<String>("t1.col.tmp");
    BufferHandle* tmp_buf = buffer_mgr.AllocateBufferHandle(tmp_base_name, tmp_buf_name, 1024);
    EXPECT_EQ(tmp_buf->GetID(), 2);
    EXPECT_EQ(tmp_buf->GetFilename(), "/tmp/infinity/_tmp/t1.col.tmp");
    ptr_t tmp_ptr = tmp_buf->LoadData();
    EXPECT_NE(tmp_ptr, nullptr);

    BufferHandle* tmp_buf1 = buffer_mgr.GetBufferHandle(nullptr, tmp_buf_name, BufferType::kTempFile);
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
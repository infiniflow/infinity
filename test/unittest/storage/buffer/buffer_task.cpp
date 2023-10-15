//
// Created by jinhai on 23-7-21.
//

#include "storage/buffer/buffer_task.h"
#include "base_test.h"
#include "main/infinity.h"
#include "storage/buffer/buffer_manager.h"
#include "storage/buffer/object_handle.h"

class BufferTaskProcessorTest : public BaseTest {};

using namespace infinity;

TEST_F(BufferTaskProcessorTest, test1) {
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

        ObjectHandle object_handle1(buf_handle1);
        ptr_t buf_ptr1 = object_handle1.GetData();
        for (i64 i = 0; i < elem_count; ++i) {
            ((i64 *)buf_ptr1)[i] = i;
        }
        SharedPtr<BufferWriteTask> write_task1 = MakeShared<BufferWriteTask>(buf_handle1);

        // file: /tmp/infinity/data/c2.col
        SharedPtr<String> obj_id2 = MakeShared<String>("c2.col");
        BufferHandle *buf_handle2 = buffer_mgr.AllocateBufferHandle(nullptr, obj_id2, elem_count * sizeof(i32));
        EXPECT_EQ(buf_handle2->GetID(), 2);
        EXPECT_EQ(buf_handle2->GetFilename(), "/tmp/infinity/_tmp/c2.col");

        ObjectHandle object_handle2(buf_handle2);
        ptr_t buf_ptr2 = object_handle2.GetData();
        for (i32 i = 0; i < elem_count; ++i) {
            ((i32 *)buf_ptr2)[i] = i + 10;
        }
        SharedPtr<BufferWriteTask> write_task2 = MakeShared<BufferWriteTask>(buf_handle2);

        AsyncBatchProcessor processor(DEFAULT_READER_PREPARE_QUEUE_SIZE, DEFAULT_READER_COMMIT_QUEUE_SIZE, BufferIO::OnPrepare, BufferIO::OnCommit);
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
    {
        BufferManager buffer_mgr(memory_limit, base_path, temp_path);
        buffer_mgr.Init();

        // file: /tmp/infinity/data/c1.col
        constexpr SizeT elem_count = 4096;

        SharedPtr<String> obj_id1 = MakeShared<String>("c1.col");
        BufferHandle *buf_handle1 = buffer_mgr.GetBufferHandle(nullptr, obj_id1, BufferType::kFile);
        EXPECT_EQ(buf_handle1->GetID(), 1);
        EXPECT_EQ(buf_handle1->GetFilename(), "/tmp/infinity/data/c1.col");

        ObjectHandle object_handle1(buf_handle1);
        ptr_t data_ptr = object_handle1.GetData();
        EXPECT_NE(data_ptr, nullptr);

        for (i64 i = 0; i < elem_count; ++i) {
            EXPECT_EQ(((i64 *)data_ptr)[i], i);
        }

        // file: /tmp/infinity/data/c2.col

        SharedPtr<String> obj_id2 = MakeShared<String>("c2.col");
        BufferHandle *buf_handle2 = buffer_mgr.GetBufferHandle(nullptr, obj_id2, BufferType::kFile);
        EXPECT_EQ(buf_handle2->GetID(), 2);
        EXPECT_EQ(buf_handle2->GetFilename(), "/tmp/infinity/data/c2.col");

        ObjectHandle object_handle2(buf_handle2);
        ptr_t data_ptr2 = object_handle2.GetData();
        EXPECT_NE(data_ptr2, nullptr);

        for (i32 i = 0; i < elem_count; ++i) {
            EXPECT_EQ(((i32 *)data_ptr2)[i], i + 10);
        }
    }
}

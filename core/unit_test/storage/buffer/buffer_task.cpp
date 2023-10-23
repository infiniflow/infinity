//
// Created by jinhai on 23-7-21.
//

#include "unit_test/base_test.h"

import infinity;
import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import table_def;
import wal_entry;
import value;
import parser;
import data_block;
import default_values;
import txn_manager;
import txn;
import base_entry;
import new_catalog;
import buffer_manager;
import buffer_handle;
import object_handle;
import buffer_task;
import async_batch_processor;

class BufferTaskProcessorTest : public BaseTest {
    void
    SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void
    TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
        system("rm -rf /tmp/infinity/");
    }
};

using namespace infinity;

TEST_F(BufferTaskProcessorTest, test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));

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

        CommonObjectHandle object_handle1(buf_handle1);
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

        CommonObjectHandle object_handle2(buf_handle2);
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

        CommonObjectHandle object_handle1(buf_handle1);
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

        CommonObjectHandle object_handle2(buf_handle2);
        ptr_t data_ptr2 = object_handle2.GetData();
        EXPECT_NE(data_ptr2, nullptr);

        for (i32 i = 0; i < elem_count; ++i) {
            EXPECT_EQ(((i32 *)data_ptr2)[i], i + 10);
        }
    }
}
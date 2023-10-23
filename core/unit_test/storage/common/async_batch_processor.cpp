//
// Created by jinhai on 23-6-2.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;
import global_resource_usage;
import third_party;
import logger;
import parser;
import async_task;
import commit_task;
import async_batch_processor;
import async_dummy_task;
import infinity;

class AsyncTaskProcessorTest : public BaseTest {
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

using namespace infinity;

SharedPtr<AsyncTask> OnPrepareTest(List<SharedPtr<AsyncTask>> &async_tasks) {
    SharedPtr<CommitTask> commit_task = MakeShared<CommitTask>(async_tasks.size());
    for (const auto &async_task : async_tasks) {
        async_task->Prepare();
        LOG_TRACE("OnPrepare: " + async_task->ToString());
        commit_task->Append(async_task.get());
    }
    return commit_task;
}

void OnCommitTest(const SharedPtr<AsyncTask> &commit_task) {
    //    async_task->Notify();
    commit_task->Commit();
    LOG_TRACE("OnCommit: " + commit_task->ToString());
}

TEST_F(AsyncTaskProcessorTest, test1) {
    using namespace infinity;
    LOG_TRACE(Format("Test name: {}.{}", test_info_->test_case_name(), test_info_->name()));
    AsyncBatchProcessor processor(10 * 1024, 1024, OnPrepareTest, OnCommitTest);
    processor.Start();

    SharedPtr<AsyncDummyTask> dummy_task1 = MakeShared<AsyncDummyTask>();
    SharedPtr<AsyncDummyTask> dummy_task2 = MakeShared<AsyncDummyTask>();
    SharedPtr<AsyncDummyTask> dummy_task3 = MakeShared<AsyncDummyTask>();
    SharedPtr<AsyncDummyTask> dummy_task4 = MakeShared<AsyncDummyTask>();
    processor.Submit(dummy_task1);
    processor.Submit(dummy_task2);
    processor.Submit(dummy_task3);
    processor.Submit(dummy_task4);

    dummy_task1->Wait();
    dummy_task2->Wait();
    dummy_task3->Wait();
    dummy_task4->Wait();

    processor.Stop();
}

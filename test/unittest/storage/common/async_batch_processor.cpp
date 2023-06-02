//
// Created by jinhai on 23-6-2.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/profiler/base_profiler.h"
#include "main/logger.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"
#include "storage/common/async_batch_processor.h"
#include "storage/common/async_dummy_task.h"
#include "storage/common/async_terminate_task.h"

class AsyncTaskProcessorTest : public BaseTest {
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

using namespace infinity;

UniquePtr<AsyncTask>
OnPrepareTest(List<UniquePtr<AsyncTask>>& async_tasks) {
    for(const auto& async_task: async_tasks) {
        LOG_TRACE("OnPrepare: " + async_task->ToString());
    }
    return MakeUnique<AsyncDummyTask>();
}

void
OnCommitTest(UniquePtr<AsyncTask>& async_task) {
    LOG_TRACE("OnCommit: " + async_task->ToString());
}

TEST_F(AsyncTaskProcessorTest, test1) {
    using namespace infinity;
    AsyncBatchProcessor processor(10 * 1024, 1024, OnPrepareTest, OnCommitTest);
    processor.Start();
    processor.Submit(MakeUnique<AsyncDummyTask>());
    processor.Submit(MakeUnique<AsyncDummyTask>());
    processor.Submit(MakeUnique<AsyncDummyTask>());
    processor.Submit(MakeUnique<AsyncDummyTask>());
    processor.Stop();
}
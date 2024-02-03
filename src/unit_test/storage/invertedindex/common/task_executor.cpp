// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "unit_test/base_test.h"
#include <iostream>
#include <storage/invertedindex/common/vespa_alloc.h>

import stl;
import logger;
import task_executor;
using namespace infinity;

class TaskExecutorTest : public BaseTest {};

int SequencedTask(Runnable &worker) {
    worker.Run();
    return 1;
}

class Executor {
public:
    UniquePtr<SequencedTaskExecutor> executor_;

    Executor() : executor_(SequencedTaskExecutor::Create(SequencedTask, 2, 1000)) {}
};

class Task {
public:
    Task() {}

    void Modify(int old_value, int new_value) {
        {
            std::lock_guard<std::mutex> guard(m_);
            if (val_ == old_value) {
                val_ = new_value;
            } else {
                ++fail_;
            }
            ++done_;
        }
        cv_.notify_all();
    }

    void Wait(int want_done) {
        std::unique_lock<std::mutex> guard(m_);
        cv_.wait(guard, [this, want_done] { return this->done_ >= want_done; });
    }

    std::mutex m_;
    std::condition_variable cv_;
    int done_{0};
    int fail_{0};
    int val_{0};
};

TEST_F(TaskExecutorTest, test1) {
    Executor executor;
    SharedPtr<Task> task = MakeShared<Task>();
    EXPECT_EQ(0, task->val_);
    executor.executor_->ExecuteLambda(0, [=]() { task->Modify(0, 20); });
    task->Wait(1);
    EXPECT_EQ(0, task->fail_);
    EXPECT_EQ(20, task->val_);
    executor.executor_->SyncAll();
    EXPECT_EQ(0, task->fail_);
    EXPECT_EQ(20, task->val_);
}

TEST_F(TaskExecutorTest, test2) {
    Executor executor;
    SharedPtr<Task> task = MakeShared<Task>();
    EXPECT_EQ(0, task->val_);

    executor.executor_->ExecuteLambda(0, [=]() {
        usleep(2000);
        task->Modify(0, 10);
    });
    executor.executor_->ExecuteLambda(0, [=]() { task->Modify(10, 20); });
    task->Wait(2);
    EXPECT_EQ(0, task->fail_);
    EXPECT_EQ(20, task->val_);
    executor.executor_->SyncAll();
    EXPECT_EQ(0, task->fail_);
    EXPECT_EQ(20, task->val_);
}

TEST_F(TaskExecutorTest, test3) {
    Executor executor;
    int try_count = 0;
    for (try_count = 0; try_count < 100; ++try_count) {
        SharedPtr<Task> task(MakeShared<Task>());
        EXPECT_EQ(0, task->val_);
        executor.executor_->ExecuteLambda(0, [=]() {
            usleep(2000);
            task->Modify(0, 10);
        });
        executor.executor_->ExecuteLambda(1, [=]() { task->Modify(10, 20); });
        task->Wait(2);
        if (task->fail_ != 1) {
            continue;
        }
        EXPECT_EQ(1, task->fail_);
        EXPECT_EQ(10, task->val_);
        executor.executor_->SyncAll();
        EXPECT_EQ(1, task->fail_);
        EXPECT_EQ(10, task->val_);
        break;
    }
    EXPECT_TRUE(try_count < 100);
}
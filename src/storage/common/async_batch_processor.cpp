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

module;

#include <string>

import stl;
import logger;
import third_party;
import async_task;
import async_terminate_task;

module async_batch_processor;

namespace infinity {

void AsyncBatchProcessor::Start() {
    prepare_worker_ = MakeUnique<Thread>(&AsyncBatchProcessor::PrepareLoop, this);
    commit_worker_ = MakeUnique<Thread>(&AsyncBatchProcessor::CommitLoop, this);
}

void AsyncBatchProcessor::Stop() {
    SharedPtr<AsyncTerminateTask> terminate_task = MakeUnique<AsyncTerminateTask>();
    prepare_queue_->Enqueue(terminate_task);
    prepare_worker_->join();
    commit_worker_->join();
}

void AsyncBatchProcessor::Submit(SharedPtr<AsyncTask> async_task) {
    LOG_TRACE("Submit task: " + async_task->ToString());
    prepare_queue_->Enqueue(Move(async_task));
}

void AsyncBatchProcessor::PrepareLoop() {
    List<SharedPtr<AsyncTask>> local_task_list{};
    List<SharedPtr<AsyncTask>> new_task_list{};
    bool running{true};

    SharedPtr<AsyncTerminateTask> terminate_task = MakeUnique<AsyncTerminateTask>();
    LOG_TRACE("Start prepare loop");
    while (running) {
        local_task_list.clear();
        new_task_list.clear();
        prepare_queue_->DequeueBulk(local_task_list);
        while (!local_task_list.empty()) {
            auto task = local_task_list.front();
            local_task_list.pop_front();
            if (task->IsTerminated()) {
                running = false;
                break;
            }
            new_task_list.emplace_back(task);
        }
        LOG_TRACE(Format("Prepare loop: {}", new_task_list.size()));
        if (!new_task_list.empty()) {
            // new_task_list isn't empty, prepare the tasks into a batch task
            SharedPtr<AsyncTask> batch_task = on_prepare_(new_task_list);
            commit_queue_->Enqueue(batch_task);
        }
        if (!running) {
            commit_queue_->Enqueue(terminate_task);
        }
    }
    LOG_TRACE("Stop prepare loop");
}

void AsyncBatchProcessor::CommitLoop() {
    List<SharedPtr<AsyncTask>> local_task_list{};
    bool running{true};
    LOG_TRACE("Start commit loop");
    while (running) {
        local_task_list.clear();
        commit_queue_->DequeueBulk(local_task_list);
        LOG_TRACE(Format("Commit loop: {}", local_task_list.size()));
        for (auto &task_ptr : local_task_list) {
            if (task_ptr->IsTerminated()) {
                running = false;
                break;
            }
            // Commit the task
            on_commit_(task_ptr);
        }
    }
    LOG_TRACE("Stop commit loop");
}

} // namespace infinity
//
// Created by jinhai on 23-6-2.
//

#include "async_batch_processor.h"
#include "async_terminate_task.h"
#include "main/logger.h"

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
    prepare_queue_->Enqueue(std::move(async_task));
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
        LOG_TRACE("Prepare loop: {}", new_task_list.size());
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
        LOG_TRACE("Commit loop: {}", local_task_list.size());
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
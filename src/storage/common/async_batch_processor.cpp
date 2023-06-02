//
// Created by jinhai on 23-6-2.
//

#include "async_batch_processor.h"
#include "main/logger.h"
#include "async_terminate_task.h"

namespace infinity {

void
AsyncBatchProcessor::Start() {

    prepare_worker_ = MakeUnique<Thread>(&AsyncBatchProcessor::PrepareLoop, this);
    commit_worker_ = MakeUnique<Thread>(&AsyncBatchProcessor::CommitLoop, this);
}

void
AsyncBatchProcessor::Stop() {
    prepare_queue_->Enqueue(MakeUnique<AsyncTerminateTask>());
    prepare_worker_->join();
    commit_worker_->join();
}

void
AsyncBatchProcessor::Submit(UniquePtr<AsyncTask> async_task) {
    LOG_TRACE("Submit task: " + async_task->ToString());
    prepare_queue_->Enqueue(std::move(async_task));
}

void
AsyncBatchProcessor::PrepareLoop() {
    List<UniquePtr<AsyncTask>> local_task_list{};
    List<UniquePtr<AsyncTask>> new_task_list{};
    bool running{true};
    LOG_TRACE("Start prepare loop");
    while(running) {
        local_task_list.clear();
        new_task_list.clear();
        prepare_queue_->DequeueBulk(local_task_list);
        while(!local_task_list.empty()) {
            auto task = std::move(local_task_list.front());
            local_task_list.pop_front();
            if(task->IsTerminated()) {
                running = false;
                break;
            }
            new_task_list.emplace_back(std::move(task));
        }
        LOG_TRACE("Prepare loop: {}", new_task_list.size());
        if(!new_task_list.empty()) {
            // new_task_list isn't empty, prepare the task into a batch task
            UniquePtr<AsyncTask> batch_task = on_prepare_(new_task_list);
            commit_queue_->Enqueue(std::move(batch_task));
        }
        if(!running) {
            commit_queue_->Enqueue(MakeUnique<AsyncTerminateTask>());
        }
    }
    LOG_TRACE("Stop prepare loop");
}

void
AsyncBatchProcessor::CommitLoop() {
    List<UniquePtr<AsyncTask>> local_task_list{};
    bool running{true};
    LOG_TRACE("Start commit loop");
    while(running) {
        local_task_list.clear();
        commit_queue_->DequeueBulk(local_task_list);
        auto task_iter = local_task_list.begin();
        LOG_TRACE("Commit loop: {}", local_task_list.size());
        while(task_iter != local_task_list.end()) {
            if(task_iter->get()->IsTerminated()) {
                running = false;
                break;
            }
            // Commit the task
            on_commit_(*task_iter);
            ++ task_iter;
        }
    }
    LOG_TRACE("Stop commit loop");
}

}
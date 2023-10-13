//
// Created by jinhai on 23-6-2.
//

#pragma once

//#include "storage/common/async_task.h"
#include "common/types/alias/containers.h"
#include "common/types/alias/primitives.h"
#include "common/types/alias/smart_ptr.h"
#include "blockingconcurrentqueue.h"
#include <condition_variable>

namespace infinity {

class AsyncTask;

class BatchBlockingQueue {
public:
    explicit BatchBlockingQueue(SizeT capacity = 1024) : capacity_(capacity) {}

    void Enqueue(SharedPtr<AsyncTask> task_ptr) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        full_cv_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push_back(std::move(task_ptr));
        empty_cv_.notify_one();
    }

    void DequeueBulk(List<SharedPtr<AsyncTask>> &output_queue) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        output_queue.splice(output_queue.end(), queue_);
        full_cv_.notify_one();
    }

    SizeT Size() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.size();
    }

    bool Empty() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.empty();
    }

protected:
    mutable std::mutex queue_mutex_{};
    std::condition_variable full_cv_{};
    std::condition_variable empty_cv_{};
    List<SharedPtr<AsyncTask>> queue_{};
    SizeT capacity_{1024};
};

} // namespace infinity

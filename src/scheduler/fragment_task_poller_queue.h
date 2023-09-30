//
// Created by jinhai on 23-9-8.
//
#pragma once

#include "common/types/alias/containers.h"
#include "blockingconcurrentqueue.h"
#include <condition_variable>

namespace infinity {

class FragmentTask;

class FragmentTaskPollerQueue {
public:
    explicit
    FragmentTaskPollerQueue(SizeT capacity = 1024) : capacity_(capacity) {
    }

    void
    Enqueue(FragmentTask* task_ptr) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        full_cv_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push_back(task_ptr);
        empty_cv_.notify_one();
    }

    void
    DequeueBulk(List<FragmentTask*>& output_queue) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        output_queue.splice(output_queue.end(), queue_);
        full_cv_.notify_one();
    }

    SizeT
    Size() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.size();
    }

    bool
    Empty() const {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        return queue_.empty();
    }

protected:
    mutable std::mutex queue_mutex_{};
    std::condition_variable full_cv_{};
    std::condition_variable empty_cv_{};
    List<FragmentTask*> queue_{};
    SizeT capacity_{32};
};

}

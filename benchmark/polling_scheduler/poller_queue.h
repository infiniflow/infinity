//
// Created by jinhai on 23-5-14.
//

#pragma once

#include "blockingconcurrentqueue.h"
#include <condition_variable>
#include <list>

namespace infinity {

struct Task;

class PollerQueue {
public:
    explicit
    PollerQueue(size_t capacity = 1024) : capacity_(capacity) {
    }

    void
    Enqueue(Task* task_ptr) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        full_cv_.wait(lock, [this] { return queue_.size() < capacity_; });
        queue_.push_back(task_ptr);
        empty_cv_.notify_one();
    }

    void
    DequeueBulk(std::list<Task*>& output_queue) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        empty_cv_.wait(lock, [this] { return !queue_.empty(); });
        output_queue.splice(output_queue.end(), queue_);
        full_cv_.notify_one();
    }

    size_t
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
    std::list<Task*> queue_{};
    size_t capacity_{32};
};

}


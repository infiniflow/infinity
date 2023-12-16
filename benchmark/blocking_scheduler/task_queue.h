//
// Created by jinhai on 23-5-9.
//

#pragma once

#include "blockingconcurrentqueue.h"

namespace infinity {

struct Task;

struct BlockingQueue {
    void
    Enqueue(Task* task) {
        queue_.enqueue(task);
    }

    template<class It>
    size_t
    DequeueBulk(It iter, size_t count) {
        return queue_.wait_dequeue_bulk(iter, count);
    }

    void
    Dequeue(Task*& task) {
        queue_.wait_dequeue(task);
    }

    moodycamel::BlockingConcurrentQueue<Task*> queue_;
};

}

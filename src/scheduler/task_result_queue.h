//
// Created by jinhai on 23-5-9.
//

#pragma once

#include "blockingconcurrentqueue.h"

namespace infinity {

class TaskResult;

struct TaskResultQueue {
    void Enqueue(TaskResult *task) { queue_.enqueue(task); }

    template <class It>
    SizeT DequeueBulk(It iter, SizeT count) {
        return queue_.wait_dequeue_bulk(iter, count);
    }

    void Dequeue(TaskResult *&task) { queue_.wait_dequeue(task); }

    moodycamel::BlockingConcurrentQueue<TaskResult *> queue_;
};

} // namespace infinity

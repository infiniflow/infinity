//
// Created by jinhai on 23-5-9.
//

#pragma once

#include "blockingconcurrentqueue.h"

namespace infinity {

class Task;

struct BlockingQueue {
    void
    Enqueue(Task* task) {
        queue_.enqueue(task);
    }

    template<class It>
    SizeT
    DequeueBulk(It iter, SizeT count) {
        return queue_.wait_dequeue_bulk(iter, count);
    }

    void
    Dequeue(Task*& task) {
        queue_.wait_dequeue(task);
    }

    moodycamel::BlockingConcurrentQueue<Task*> queue_;
};

}

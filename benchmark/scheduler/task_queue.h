//
// Created by jinhai on 23-5-9.
//

#pragma once

#include "blockingconcurrentqueue.h"
#include "common/utility/mpsc_queue.h"
#include "task.h"

namespace infinity {

struct BlockingQueue {
    void
    Enqueue(Task *task) {
        queue_.enqueue(task);
    }

    void
    Dequeue(Task *&task) {
        queue_.wait_dequeue(task);
    }

    moodycamel::BlockingConcurrentQueue<Task *> queue_;
};

struct ConcurrentQueue {
    bool
    TryEnqueue(Task *task) {
        return queue_.try_enqueue(task);
    }

    bool
    TryDequeue(Task *&task) {
        return queue_.try_dequeue(task);
    }

    moodycamel::ConcurrentQueue<Task *> queue_;
};

struct WaitFreeQueue {
    void
    TryEnqueue(Task *task) {
        queue_.enqueue(task);
    }

    bool
    TryDequeue(Task *&task) {
        return queue_.dequeue(task);
    }

    MPSCQueue<Task *> queue_;
};

}

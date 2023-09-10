//
// Created by jinhai on 23-9-8.
//


#pragma once

#include "common/types/internal_types.h"
#include "blockingconcurrentqueue.h"

namespace infinity {

class FragmentTask;
struct FragmentTaskBlockQueue {
    void
    Enqueue(FragmentTask *task) {
        queue_.enqueue(task);
    }

    template<class It>
    void
    EnqueueBulk(It iter, SizeT count) {
        queue_.enqueue_bulk(std::forward<It>(iter), count);
    }

    void
    Dequeue(FragmentTask *&task) {
        queue_.wait_dequeue(task);
    }

    moodycamel::BlockingConcurrentQueue<FragmentTask *> queue_;
};

}

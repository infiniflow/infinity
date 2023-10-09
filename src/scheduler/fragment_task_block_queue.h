//
// Created by jinhai on 23-9-8.
//

#pragma once

#include "blockingconcurrentqueue.h"
#include "common/types/alias/primitives.h"

namespace infinity {

class FragmentTask;

struct FragmentTaskBlockQueue {
    void Enqueue(FragmentTask *task) { queue_.enqueue(task); }

    template <class It>
    void EnqueueBulk(It iter, SizeT count) {
        queue_.enqueue_bulk(std::forward<It>(iter), count);
    }

    void Dequeue(FragmentTask *&task) { queue_.wait_dequeue(task); }

    moodycamel::BlockingConcurrentQueue<FragmentTask *> queue_;
};

} // namespace infinity

//
// Created by jinhai on 23-9-14.
//

#pragma once

#include "blockingconcurrentqueue.h"
#include "scheduler/fragment_data.h"

namespace infinity {

struct FragmentDataQueue {
    inline void Enqueue(const SharedPtr<FragmentData> &fragment_data) { queue_.enqueue(fragment_data); }

    template <class It>
    inline void EnqueueBulk(It iter, SizeT count) {
        queue_.enqueue_bulk(std::forward<It>(iter), count);
    }

    inline bool TryDequeue(SharedPtr<FragmentData> &fragment_data) { return queue_.try_dequeue(fragment_data); }

    inline void Dequeue(SharedPtr<FragmentData> &fragment_data) { queue_.wait_dequeue(fragment_data); }

    inline SizeT ApproxSize() const { return queue_.size_approx(); }

    moodycamel::BlockingConcurrentQueue<SharedPtr<FragmentData>> queue_;
};

} // namespace infinity
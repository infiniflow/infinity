//
// Created by jinhai on 23-9-14.
//

#pragma once


#include "blockingconcurrentqueue.h"

namespace infinity {

class DataBlock;

struct FragmentDataQueue {
    inline void
    Enqueue(const SharedPtr<DataBlock>& data_block) {
        queue_.enqueue(data_block);
    }

    template<class It>
    inline void
    EnqueueBulk(It iter, SizeT count) {
        queue_.enqueue_bulk(std::forward<It>(iter), count);
    }

    inline bool
    TryDequeue(SharedPtr<DataBlock>& data_block) {
        return queue_.try_dequeue(data_block);
    }

    inline void
    Dequeue(SharedPtr<DataBlock>& data_block) {
        queue_.wait_dequeue(data_block);
    }

    inline SizeT
    ApproxSize() const {
        return queue_.size_approx();
    }

    moodycamel::BlockingConcurrentQueue<SharedPtr<DataBlock>> queue_;
};

}
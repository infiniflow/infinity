//
// Created by jinhai on 23-5-9.
//

#pragma once

#include "blockingconcurrentqueue.h"
#include "common/utility/mpsc_queue.h"

namespace infinity {

struct ConcurrentQueue {
    bool
    TryEnqueue(SharedPtr<Buffer> buffer) {
        return queue_.try_enqueue(std::move(buffer));
    }

    bool
    TryDequeue(SharedPtr<Buffer>& buffer) {
        return queue_.try_dequeue(buffer);
    }

    moodycamel::ConcurrentQueue<SharedPtr<Buffer>> queue_{};
};

struct WaitFreeQueue {
    void
    TryEnqueue(SharedPtr<Buffer> buffer) {
        queue_.enqueue(std::move(buffer));
    }

    bool
    TryDequeue(SharedPtr<Buffer>& buffer) {
        return queue_.dequeue(buffer);
    }

    MPSCQueue<SharedPtr<Buffer>> queue_{};
};

}

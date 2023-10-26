//
// Created by jinhai on 23-5-9.
//

#pragma once

#include "mpsc_queue.h"
#include "concurrentqueue.h"
#include <memory>

namespace infinity {

struct ConcurrentQueue {
    bool
    TryEnqueue(std::shared_ptr<Buffer> buffer) {
        return queue_.try_enqueue(std::move(buffer));
    }

    bool
    TryDequeue(std::shared_ptr<Buffer>& buffer) {
        return queue_.try_dequeue(buffer);
    }

    moodycamel::ConcurrentQueue<std::shared_ptr<Buffer>> queue_{};
};

struct WaitFreeQueue {
    void
    TryEnqueue(std::shared_ptr<Buffer> buffer) {
        queue_.enqueue(std::move(buffer));
    }

    bool
    TryDequeue(std::shared_ptr<Buffer>& buffer) {
        return queue_.dequeue(buffer);
    }

    MPSCQueue<std::shared_ptr<Buffer>> queue_{};
};

}

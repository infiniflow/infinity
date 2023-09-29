#include "channel.h"

namespace infinity {

Channel::Channel(const std::uint16_t worker_id)
        : id_(worker_id) {

}

void
Channel::Put(PipelineTaskPtr task) {
    queue_.enqueue(task);
}

PipelineTaskPtr
Channel::Take() {
    PipelineTaskPtr task;
    while(queue_.dequeue(task)) {
        return task;
    }
    return nullptr;
}

}
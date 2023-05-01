#include "channel.h"

namespace infinity {

Channel::Channel(const std::uint16_t worker_id)
    :id_(worker_id) {

}

void Channel::Put(PipelineTaskPtr task) {
    queue_.push_back(&task);
}

PipelineTaskPtr Channel::Take() {
    while(!queue_.empty()) {
        PipelineTaskPtr *task = queue_.pop_front();
        return *task;
    }
    return nullptr;
}

}
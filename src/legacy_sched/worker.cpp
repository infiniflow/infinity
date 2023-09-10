#include "worker.h"
#include "common/utility/builtin.h"

namespace infinity {

Worker::Worker(std::uint16_t id, std::uint16_t cpu_id)
    :id_(id)
    ,cpu_id_(cpu_id)
    ,channel_(id)
    ,is_running_(false) {
}

void Worker::Execute() {
    while(is_running_ == false) {
        Builtin::pause();
    }

    PipelineTaskPtr task;

    while(is_running_) {
        while((task = channel_.Take()) != nullptr) {
            //execution
        }
    }
}
}
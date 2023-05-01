#pragma once

#include "pipeline_task.h"
#include "common/utility/mpsc_queue.h"

#include <cstdint>

namespace infinity {
class Channel {
public:
    Channel(const std::uint16_t worker_id);

    ~Channel() noexcept = default;

    [[nodiscard]] std::uint16_t ID() const {
        return id_;
    }

    void Put(PipelineTaskPtr task);

    PipelineTaskPtr Take();

private:
    const std::uint16_t id_;

    MPSCQueue<PipelineTaskPtr> queue_;
};
}
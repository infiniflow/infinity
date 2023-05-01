#pragma once

#include "worker_manager.h"
#include "main/query_context.h"

namespace infinity {
class PipelineExecutor {
public:
    PipelineExecutor(std::uint16_t max_cores);

    ~PipelineExecutor() noexcept;

    void Run();

private:
    WorkerManager worker_manager_;
};
}
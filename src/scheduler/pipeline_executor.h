#pragma once

#include "worker_directory.h"
#include "main/query_context.h"

namespace infinity{
class PipelineExecutor{
public:
	PipelineExecutor(std::uint16_t max_cores);

	~PipelineExecutor() noexcept;

    void Run();

private:
	WorkerDirectory worker_directory_;
};
}
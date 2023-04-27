#pragma once

#include "common/utility/topology.h"
#include "worker.h"

#include <array>

namespace infinity{
class PipelineExecutor{
public:
	PipelineExecutor(std::uint16_t max_cores);

	~PipelineExecutor() noexcept;

    void Run();

private:
	std::uint16_t max_cores_;

	Topology cpu_topology_;

	std::array<Worker *, Topology::MaxCores> workers_;
};
}
#pragma once

#include "pipeline_task.h"

#include <cstdint>
#include <queue>

namespace infinity{
class Channel{
public:
	Channel(const std::uint16_t worker_id);

	~Channel() noexcept = default;

	[[nodiscard]] std::uint16_t ID() const { 
		return id_; 
	}

	void Put(const PipelineTaskPtr task);

	PipelineTaskPtr Take();

private:
	const std::uint16_t id_;

	std::deque<PipelineTaskPtr> queue_;
};
}
#pragma once

namespace infinity{

class PipelineTask{
public:
	PipelineTask();

	~PipelineTask() noexcept = default;
private:
};

using PipelineTaskPtr = PipelineTask*;

}
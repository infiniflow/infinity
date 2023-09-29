#include "pipeline_task.h"

namespace infinity {
PipelineTask::PipelineTask(
        SharedPtr<Pipeline>& pipeline, std::uint16_t index)
        : index_(index), pipeline_(pipeline) {

}

void
PipelineTask::Execute() {

}

void
PipelineTask::Close() {

}

void
PipelineTask::Finalize() {

}

}
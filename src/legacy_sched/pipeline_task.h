#pragma once

#include "executor/physical_operator.h"
#include "pipeline.h"

#include <vector>

namespace infinity {

class PipelineTask {
public:
    PipelineTask(SharedPtr<Pipeline> &pipeline, std::uint16_t index);

    ~PipelineTask() noexcept = default;

    void Execute();

    void Close();

    void Finalize();

private:
    std::uint16_t index_;
    SharedPtr<Pipeline> pipeline_;
    std::vector<SharedPtr<PhysicalOperator>> operators_;
    SharedPtr<PhysicalOperator> source_;
    SharedPtr<PhysicalOperator> sink_;
    SharedPtr<PhysicalOperator> root_;
};

using PipelineTaskPtr = PipelineTask *;

} // namespace infinity
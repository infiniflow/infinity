#include "pipeline_executor.h"


namespace infinity {

PipelineExecutor::PipelineExecutor(std::uint16_t max_cores) {
    worker_manager_.CreateWorkers(max_cores);
}

PipelineExecutor::~PipelineExecutor() {

}

void PipelineExecutor::Run() {
    worker_manager_.Start();
}

}
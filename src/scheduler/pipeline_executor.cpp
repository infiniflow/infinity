#include "pipeline_executor.h"


namespace infinity{

PipelineExecutor::PipelineExecutor(std::uint16_t max_cores){
	worker_directory_.CreateWorkers(max_cores);
}

PipelineExecutor::~PipelineExecutor(){

}

void PipelineExecutor::Run(){
	worker_directory_.Start();
}

}
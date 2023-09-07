#include "pipeline_executor.h"


namespace infinity {

PipelineExecutor::PipelineExecutor(std::uint16_t max_cores) {
    worker_manager_.CreateWorkers(max_cores);
}

void PipelineExecutor::Run() {
    worker_manager_.Start();
}

bool PipelineExecutor::AdmitQuery(
    QueryContext* query_context,
    const SharedPtr<Pipeline> &pipeline){
    //Judge according to load
    if(policy_enforcer_.AdmitQuery(query_context)){
        //build pipeline tasks

        return true;
    }
    return false;
}

bool PipelineExecutor::CancelQuery(QueryContext* query_context){
    return false;
}

void PipelineExecutor::DispatchTask(std::uint16_t worker_id, PipelineTaskPtr task){
    Channel& channel = worker_manager_.GetWorker(worker_id)->GetChannel();
    channel.Put(task);
}

}
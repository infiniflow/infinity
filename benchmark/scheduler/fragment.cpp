//
// Created by jinhai on 23-5-9.
//

#include "fragment.h"

namespace infinity {

UniquePtr<Task>
Fragment::BuildTask(u64 parallel_size) {
    assert(parallel_size > 0);
    assert(this->source_ != nullptr);
    assert(this->sink_ != nullptr);

    // Build sink task firstly
    UniquePtr<SinkTask> sink_task = MakeUnique<SinkTask>(this->sink_.get());

    if(this->source_->type() == SourceType::kScan) {
        assert(this->child_ == nullptr);
        Vector<UniquePtr<PipelineTask>> pipeline_tasks;
        for(u64 i = 0; i < parallel_size; ++ i) {
            UniquePtr<PipelineTask> pipeline_task = MakeUnique<PipelineTask>(this->source_.get());
            for(auto& op: this->operators_) {
                pipeline_task->AddOperator(op.get());
            }
            // Set sink task have parallel size input task
//            sink_task->AddChild(std::move(pipeline_task));
        }
    } else {
        // SourceType::kExchange
        UniquePtr<Task> child_task = this->child_ != nullptr ?
                                     this->child_->BuildTask(parallel_size) : nullptr;

        assert(child_task->type() == TaskType::kSink);


        SharedPtr<ExchangeTask> exchange_task = MakeShared<ExchangeTask>(this->source_.get());
        exchange_task->SetChild(std::move(child_task));
        ((SinkTask*)(child_task.get()))->SetParent(exchange_task.get());

        Vector<UniquePtr<PipelineTask>> pipeline_tasks;
        for(u64 i = 0; i < parallel_size; ++ i) {
            UniquePtr<PipelineTask> pipeline_task = MakeUnique<PipelineTask>(this->source_.get());
            for(auto& op: this->operators_) {
                pipeline_task->AddOperator(op.get());
            }

            pipeline_task->SetChild(exchange_task);
            exchange_task->AddParent(pipeline_task.get());

//            sink_task->AddChild(std::move(pipeline_task));
        }

    }
    return sink_task;
}

}
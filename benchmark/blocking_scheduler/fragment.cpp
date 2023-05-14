//
// Created by jinhai on 23-5-9.
//

#include "fragment.h"

namespace infinity {

Vector<SharedPtr<Task>>
Fragment::BuildTask(u64 parallel_size) {
    assert(parallel_size > 0);
    assert(this->source_ != nullptr);
    assert(this->sink_ != nullptr);

    Vector<SharedPtr<Task>> child_tasks;
    if(this->child_ != nullptr) {
        child_tasks = this->child_->BuildTask(parallel_size);
    }

    Vector<SharedPtr<Task>> result;

    SizeT task_count = 0;
    if(fragment_type_ == FragmentType::kSerial) {
        task_count = 1;
    } else if(fragment_type_ == FragmentType::kParallel) {
        task_count = parallel_size;
    }

    for(SizeT idx = 0; idx < task_count; ++ idx) {
        result.emplace_back(MakeShared<PipelineTask>());
        PipelineTask* the_task = (PipelineTask*)(result[idx].get());
        the_task->AddSink(sink_.get());
        the_task->AddSource(source_.get(), !child_tasks.empty());
        for(auto& op: this->operators_) {
            the_task->AddOperator(op.get());
        }
        the_task->SetChildren(child_tasks);
    }
    return result;
}

}
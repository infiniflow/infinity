//
// Created by jinhai on 23-9-8.
//

#include "fragment_task.h"
#include "fragment_context.h"

namespace infinity {

void
FragmentTask::Init() {
    FragmentContext* fragment_context = (FragmentContext*)fragment_context_;
    // Init each operator input / output
    operator_input_state_.resize(operator_count_);
    operator_output_state_.resize(operator_count_);
}

void
FragmentTask::OnExecute(i64 worker_id) {
    LOG_TRACE("Execute fragment task on {}", worker_id);
    FragmentContext* fragment_context = (FragmentContext*)fragment_context_;

    // TODO:
    // Tell the fragment type:
    // For materialized type, we need to run the sink on the last source
    //     - Each execution will fetch data or data reference from source
    //     - Source operator will indicate the last execution
    // For streaming type, we need to run sink each execution

    PhysicalSource* source_op = fragment_context->GetSourceOperator();
    source_op->Execute(fragment_context->query_context(), source_state_.get());

    Vector<PhysicalOperator*>& operator_refs = fragment_context->GetOperators();

    UniquePtr<String> err_msg = nullptr;
    try {
        for(i64 op_idx = operator_count_ - 1; op_idx >= 0; --op_idx) {
            operator_refs[op_idx]->Execute(fragment_context->query_context(),
                                           operator_input_state_[op_idx].get(),
                                           operator_output_state_[op_idx].get());
        }
    } catch(const std::exception& e) {
        err_msg = MakeUnique<String>(e.what());
    }

    if(sink_state_->error_message_ == nullptr and err_msg != nullptr) {
        sink_state_->error_message_ = std::move(err_msg);
    } else {
        PhysicalSink* sink_op = fragment_context->GetSinkOperator();
        sink_op->Execute(fragment_context->query_context(), sink_state_.get());
    }
}

bool
FragmentTask::Ready() const {
    FragmentContext* fragment_context = (FragmentContext*)fragment_context_;
    PhysicalSource* source_op = fragment_context->GetSourceOperator();
    return source_op->ReadyToExec(source_state_.get());
}

bool
FragmentTask::Complete() const {
    FragmentContext* fragment_context = (FragmentContext*)fragment_context_;
    PhysicalSink* sink_op = fragment_context->GetSinkOperator();
    if(sink_state_->prev_output_state_->Complete() && sink_op->sink_type() == SinkType::kResult) {
        fragment_context->Complete();
    }
    return true;
}

}
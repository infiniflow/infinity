// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <sstream>

import fragment_context;
import profiler;
import plan_fragment;
import stl;
import third_party;
import logger;
import physical_source;
import physical_sink;
import physical_operator;
import infinity_exception;
import operator_state;
import physical_operator_type;

module fragment_task;

namespace infinity {

void FragmentTask::Init() {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    // Init each operator input / output
    operator_states_.resize(operator_count_);
}

void FragmentTask::OnExecute(i64 worker_id) {
    LOG_TRACE(Format("Execute fragment task on {}, {}", worker_id, this->PhysOpsToString()));
    //    infinity::BaseProfiler prof;
    //    prof.Begin();
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    QueryContext *query_context = fragment_context->query_context();

    // TODO:
    // Tell the fragment type:
    // For materialized type, we need to run the sink on the last source
    //     - Each execution will fetch data or data reference from source
    //     - Source operator will indicate the last execution
    // For streaming type, we need to run sink each execution

    PhysicalSource *source_op = fragment_context->GetSourceOperator();

    LOG_TRACE(Format("Start execute source operator {}, {}", worker_id, this->PhysOpsToString()));

    source_op->Execute(fragment_context->query_context(), source_state_.get());
    LOG_TRACE(Format("End execute source operator"));

    Vector<PhysicalOperator *> &operator_refs = fragment_context->GetOperators();

    bool enable_profiler = query_context->is_enable_profiling();
    TaskProfiler profiler(TaskBinding(), enable_profiler, operator_count_);
    profiler.Begin();
    UniquePtr<String> err_msg = nullptr;
    try {
        for (i64 op_idx = operator_count_ - 1; op_idx >= 0; --op_idx) {
            LOG_TRACE(Format("Start execute operator {}", op_idx));
            profiler.StartOperator(operator_refs[op_idx]);
            operator_refs[op_idx]->Execute(fragment_context->query_context(), operator_states_[op_idx].get());
            profiler.StopOperator(operator_states_[op_idx].get());
            LOG_TRACE(Format("Finish execute operator {}", op_idx));
        }
    } catch (const Exception &e) {
        err_msg = MakeUnique<String>(e.what());
    }
    profiler.End();
    if (sink_state_->error_message_.get() == nullptr and err_msg.get() != nullptr) {
        sink_state_->error_message_ = Move(err_msg);
    } else {
        PhysicalSink *sink_op = fragment_context->GetSinkOperator();

        fragment_context->FlushProfiler(profiler);
        sink_op->Execute(query_context, sink_state_.get());
    }

    //    prof.End();
    //    LOG_TRACE(prof.ElapsedToString());
    LOG_TRACE(Format("Finish execute fragment task on {}", worker_id));
}

u64 FragmentTask::ProposedCPUID(u64 max_cpu_count) const {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    return (fragment_context->query_context()->GetTxn()->TxnID() + task_id_) % max_cpu_count;
}

bool FragmentTask::Ready() const {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    PhysicalSource *source_op = fragment_context->GetSourceOperator();
    return source_op->ReadyToExec(source_state_.get());
}

bool FragmentTask::IsComplete() const {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    PhysicalSink *sink_op = fragment_context->GetSinkOperator();
    return sink_state_->prev_op_state_->Complete();
}

TaskBinding FragmentTask::TaskBinding() const {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    struct TaskBinding binding {};

    binding.task_id_ = task_id_;
    binding.fragment_id_ = fragment_context->fragment_ptr()->FragmentID();
    return binding;
}

void FragmentTask::TryCompleteFragment() {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    fragment_context->FinishTask();
}

String FragmentTask::PhysOpsToString() {
    std::stringstream ss;

    for (const UniquePtr<OperatorState> &op : operator_states_) {
        ss << PhysicalOperatorToString(op->operator_type_) << " ";
    }
    return ss.str();
}

} // namespace infinity

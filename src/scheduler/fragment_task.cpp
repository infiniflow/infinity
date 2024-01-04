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
import query_context;
import base_table_ref;
import defer_op;

module fragment_task;

namespace infinity {

void FragmentTask::Init() {
    //    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    // Init each operator input / output
    operator_states_.resize(operator_count_);
}

void FragmentTask::OnExecute(i64) {
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

    bool execute_success{false};
    bool source_complete = source_op->Execute(fragment_context->query_context(), source_state_.get());
    if(source_state_->error_message_.get() == nullptr) {
        // No source error
        Vector<PhysicalOperator *> &operator_refs = fragment_context->GetOperators();

        bool enable_profiler = query_context->is_enable_profiling();
        TaskProfiler profiler(TaskBinding(), enable_profiler, operator_count_);
        HashMap<SizeT, SharedPtr<BaseTableRef>> table_refs;
        profiler.Begin();
        UniquePtr<String> err_msg = nullptr;
        try {
            for (i64 op_idx = operator_count_ - 1; op_idx >= 0; --op_idx) {
                profiler.StartOperator(operator_refs[op_idx]);
                DeferFn defer_fn([&]() { profiler.StopOperator(operator_states_[op_idx].get()); });

                operator_refs[op_idx]->InputLoad(fragment_context->query_context(), operator_states_[op_idx].get(), table_refs);
                execute_success = operator_refs[op_idx]->Execute(fragment_context->query_context(), operator_states_[op_idx].get());
                operator_refs[op_idx]->FillingTableRefs(table_refs);

                if (!execute_success) {
                    break;
                }
            }
        } catch (const Exception &e) {
            err_msg = MakeUnique<String>(e.what());
        }
        profiler.End();
        fragment_context->FlushProfiler(profiler);

        if (err_msg.get() != nullptr) {
            sink_state_->error_message_ = Move(err_msg);
            this->set_status(FragmentTaskStatus::kError);
        }
    }

    if(source_complete && source_state_->error_message_.get() != nullptr) {
        sink_state_->error_message_ = Move(source_state_->error_message_);
        this->set_status(FragmentTaskStatus::kError);
    }

    if (execute_success or sink_state_->error_message_.get() != nullptr) {
        PhysicalSink *sink_op = fragment_context->GetSinkOperator();
        sink_op->Execute(query_context, fragment_context, sink_state_.get());
    }
}

bool FragmentTask::Ready() const {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    PhysicalSource *source_op = fragment_context->GetSourceOperator();
    return source_op->ReadyToExec(source_state_.get());
}

bool FragmentTask::IsComplete() const { return sink_state_->prev_op_state_->Complete() or status() == FragmentTaskStatus::kError; }

TaskBinding FragmentTask::TaskBinding() const {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    struct TaskBinding binding {};

    binding.task_id_ = task_id_;
    binding.fragment_id_ = fragment_context->fragment_ptr()->FragmentID();
    return binding;
}

void FragmentTask::TryCompleteFragment() {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    LOG_TRACE(Format("Task: {} of Fragment: {} is completed", task_id_, fragment_context->fragment_ptr()->FragmentID()));
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

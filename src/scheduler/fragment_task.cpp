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

import fragment_context;
import stl;
import third_party;
import logger;
import physical_source;
import physical_sink;
import physical_operator;
import infinity_exception;

module fragment_task;

namespace infinity {

void FragmentTask::Init() {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    // Init each operator input / output
    operator_states_.resize(operator_count_);
}

void FragmentTask::OnExecute(i64 worker_id) {
    LOG_TRACE(Format("Execute fragment task on {}", worker_id));
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;

    // TODO:
    // Tell the fragment type:
    // For materialized type, we need to run the sink on the last source
    //     - Each execution will fetch data or data reference from source
    //     - Source operator will indicate the last execution
    // For streaming type, we need to run sink each execution

    PhysicalSource *source_op = fragment_context->GetSourceOperator();
    source_op->Execute(fragment_context->query_context(), source_state_.get());

    Vector<PhysicalOperator *> &operator_refs = fragment_context->GetOperators();

    UniquePtr<String> err_msg = nullptr;
    try {
        for (i64 op_idx = operator_count_ - 1; op_idx >= 0; --op_idx) {
            operator_refs[op_idx]->Execute(fragment_context->query_context(),
                                           operator_states_[op_idx].get());
        }
    } catch (const Exception &e) {
        err_msg = MakeUnique<String>(e.what());
    }

    if (sink_state_->error_message_.get() == nullptr and err_msg.get() != nullptr) {
        sink_state_->error_message_ = Move(err_msg);
    } else {
        PhysicalSink *sink_op = fragment_context->GetSinkOperator();
        sink_op->Execute(fragment_context->query_context(), sink_state_.get());
    }
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

void FragmentTask::TryCompleteFragment() {
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    fragment_context->FinishTask();
}
} // namespace infinity

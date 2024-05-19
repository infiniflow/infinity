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

module fragment_task;

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
import fragment_context;
import status;
import parser_assert;

namespace infinity {

void FragmentTask::Init() {
    //    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    // Init each operator input / output
    operator_states_.resize(operator_count_);
}

void FragmentTask::OnExecute() {
    LOG_TRACE(fmt::format("Task: {} of Fragment: {} is running", task_id_, FragmentId()));
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
    if(source_state_->state_type_ == SourceStateType::kQueue) {
        // For debug
        LOG_TRACE(PhysOpsToString());
    }

    bool execute_success{false};
    source_op->Execute(query_context, source_state_.get());
    Status operator_status{};
    if (source_state_->status_.ok()) {
        // No source error
        Vector<PhysicalOperator *> &operator_refs = fragment_context->GetOperators();

        bool enable_profiler = query_context->is_enable_profiling();
        TaskProfiler profiler(TaskBinding(), enable_profiler, operator_count_);
        HashMap<SizeT, SharedPtr<BaseTableRef>> table_refs;
        profiler.Begin();
        try {
            for (i64 op_idx = operator_count_ - 1; op_idx >= 0; --op_idx) {
                profiler.StartOperator(operator_refs[op_idx]);
                DeferFn defer_fn([&]() { profiler.StopOperator(operator_states_[op_idx].get()); });

                operator_refs[op_idx]->InputLoad(query_context, operator_states_[op_idx].get(), table_refs);
                execute_success = operator_refs[op_idx]->Execute(query_context, operator_states_[op_idx].get());
                operator_refs[op_idx]->FillingTableRefs(table_refs);

                if (!operator_states_[op_idx]->status_.ok()) {
                    operator_status = operator_states_[op_idx]->status_;
                    break;
                }
                if (!execute_success) {
                    break;
                }
            }
        } catch (RecoverableException &e) {
            LOG_ERROR(e.what());
            operator_status = Status(e.ErrorCode(), e.what());
        } catch (ParserException &e) {
            LOG_ERROR(e.what());
            operator_status = Status::ParserError(e.what());
        } catch (UnrecoverableException &e) {
            LOG_CRITICAL(e.what());
            throw e;
        }

        profiler.End();
        fragment_context->FlushProfiler(profiler);
    }

    if (!operator_status.ok()) {
        sink_state_->status_ = operator_status;
        status_ = FragmentTaskStatus::kError;
    } else if (execute_success) {
        PhysicalSink *sink_op = fragment_context->GetSinkOperator();
        sink_op->Execute(query_context, fragment_context, sink_state_.get());
    }
}

u64 FragmentTask::FragmentId() const {
    auto *fragment_context = static_cast<FragmentContext *>(fragment_context_);
    return fragment_context->fragment_ptr()->FragmentID();
}

// Finished **OR** Error
bool FragmentTask::IsComplete() { return sink_state_->prev_op_state_->Complete(); }

bool FragmentTask::TryIntoWorkerLoop() {
    std::unique_lock lock(mutex_);
    if (status_ != FragmentTaskStatus::kPending) {
        return false;
    }
    status_ = FragmentTaskStatus::kRunning;
    return true;
}

// Stream fragment source has no data
bool FragmentTask::QuitFromWorkerLoop() {
    // return false; // FIXME
    // If reach here, child fragment must be stream
    if (source_state_->state_type_ != SourceStateType::kQueue) {
        // fragment's source is not from queue
        return false;
    }
    auto *queue_state = static_cast<QueueSourceState *>(source_state_.get());

    std::unique_lock lock(mutex_);
    if (queue_state->source_queue_.Empty() && status_ == FragmentTaskStatus::kRunning) {
        status_ = FragmentTaskStatus::kPending;
        LOG_TRACE(fmt::format("Task: {} of Fragment: {} quits from worker loop", task_id_, FragmentId()));
        return true;
    }
    LOG_TRACE(fmt::format("Task: {} of Fragment: {} is still running", task_id_, FragmentId()));
    return false;
}

TaskBinding FragmentTask::TaskBinding() const {
    struct TaskBinding binding {};

    binding.task_id_ = task_id_;
    binding.fragment_id_ = FragmentId();
    return binding;
}

bool FragmentTask::CompleteTask() {
    // One thread reach here
    if (status_ == FragmentTaskStatus::kRunning) {
        status_ = FragmentTaskStatus::kFinished;
    } else {
        UnrecoverableError("Status should be error");
    }
    FragmentContext *fragment_context = (FragmentContext *)fragment_context_;
    LOG_TRACE(fmt::format("Task: {} of Fragment: {} is completed", task_id_, FragmentId()));
    return fragment_context->TryFinishFragment();
}

String FragmentTask::PhysOpsToString() {
    std::stringstream ss;

    for (const UniquePtr<OperatorState> &op : operator_states_) {
        ss << PhysicalOperatorToString(op->operator_type_) << " ";
    }
    return ss.str();
}

FragmentContext *FragmentTask::fragment_context() const { return reinterpret_cast<FragmentContext *>(fragment_context_); }

} // namespace infinity

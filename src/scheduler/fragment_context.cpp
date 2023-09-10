//
// Created by jinhai on 23-9-8.
//

#include "fragment_context.h"
#include "src/executor/fragment/plan_fragment.h"

namespace infinity {

UniquePtr<FragmentContext>
FragmentContext::MakeFragmentContext(QueryContext* query_context, PlanFragment* fragment_ptr) {
    Vector<PhysicalOperator*>& fragment_ops = fragment_ptr->GetOperators();
    switch(fragment_ops[0]->operator_type()) {
        case PhysicalOperatorType::kInvalid: {
            PlannerError("Invalid physical operator type")
        }
        case PhysicalOperatorType::kCreateTable:
        case PhysicalOperatorType::kCreateCollection:
        case PhysicalOperatorType::kCreateSchema:
        case PhysicalOperatorType::kCreateView:
        case PhysicalOperatorType::kDropTable:
        case PhysicalOperatorType::kDropCollection:
        case PhysicalOperatorType::kDropSchema:
        case PhysicalOperatorType::kDropView: {
            if(fragment_ops.size() == 1) {
                // Only one operator
                // These operator only need one CPU to run
                UniquePtr<GlobalMaterializedFragmentCtx> fragment_context
                        = MakeUnique<GlobalMaterializedFragmentCtx>(fragment_ptr, query_context);
                UniquePtr<FragmentTask> fragment_task = MakeUnique<FragmentTask>(fragment_context.get());
                fragment_context->AddTask(std::move(fragment_task), MakeUnique<DDLInputState>(), MakeUnique<DDLOutputState>());
                return fragment_context;
            } else {
                SchedulerError("Not support more one operator fragment")
            }
        }
        case PhysicalOperatorType::kAlter:
        case PhysicalOperatorType::kExplain:
        case PhysicalOperatorType::kPreparedPlan:
        case PhysicalOperatorType::kShow:
        case PhysicalOperatorType::kFlush: {
            if(fragment_ops.size() == 1) {
                // Only one operator
                // These operator only need one CPU to run
                UniquePtr<GlobalMaterializedFragmentCtx> fragment_context
                        = MakeUnique<GlobalMaterializedFragmentCtx>(fragment_ptr, query_context);
                UniquePtr<FragmentTask> fragment_task = MakeUnique<FragmentTask>(fragment_context.get());

                // FIXME
                fragment_context->AddTask(std::move(fragment_task), nullptr, nullptr);
                return fragment_context;
            } else {
                SchedulerError("Not support more one operator fragment")
            }
        }
        default: {
            SchedulerError("Not supported operator type")
        }
    }

    return nullptr;
}

Vector<PhysicalOperator*>&
FragmentContext::GetOperators() {
    return fragment_ptr_->GetOperators();
}

void
FragmentContext::AddTask(UniquePtr<FragmentTask> task,
                         UniquePtr<InputState> input_state,
                         UniquePtr<OutputState> output_state) {
    std::unique_lock<std::mutex> locker(locker_);
    FragmentTask* task_ptr = task.get();
    tasks_.emplace_back(std::move(task));
    task_ptr->SetInputOutputState(input_state.get(), output_state.get());

    input_states_.emplace_back(std::move(input_state));
    output_states_.emplace_back(std::move(output_state));
    // TODO: let the input state buffer shared with previous output state buffer.
    InitializeOutput(task_ptr, tasks_.size() - 1);
}

void
GlobalMaterializedFragmentCtx::InitializeOutput(FragmentTask* fragment_task, u64 task_id) {
    // Get the last output of the last op of the task
    // SharedPtr<Vector<String>> output_names
    // let the input state buffer shared with previous output state buffer.
}

SharedPtr<Table>
GlobalMaterializedFragmentCtx::GetResultInternal() {
    if(tasks_.size() == 1) {
        // One operator fragment
        switch(output_states_[0]->state_type_) {
            case OperatorStateType::kDDL: {
                DDLOutputState* ddl_output_state = (DDLOutputState*)(output_states_[0].get());
                if(ddl_output_state->error_message_ != nullptr) {
                    ExecutorError(*ddl_output_state->error_message_);
                } else {
                    // Success
                    Vector<SharedPtr<ColumnDef>> column_defs = {
                            MakeShared<ColumnDef>(0,
                                                  fragment_ptr_->GetOperators()[0]->GetOutputTypes()->at(0),
                                                  fragment_ptr_->GetOperators()[0]->GetOutputNames()->at(0),
                                                  HashSet<ConstraintType>())
                    };

                    return Table::MakeResultTable(column_defs);
                }
            }
            case OperatorStateType::kInvalid: {
                ExecutorError("Invalid operator state type")
            }
        }
    }
    NotImplementError("GlobalMaterializedFragmentCtx::GetResultInternal")
}

void
LocalMaterializedFragmentCtx::InitializeOutput(FragmentTask* fragment_task, u64 task_id) {
    // Get the last output of the last op of the task
}

SharedPtr<Table>
LocalMaterializedFragmentCtx::GetResultInternal() {
    NotImplementError("LocalMaterializedFragmentCtx::GetResultInternal")
}

void
StreamFragmentCtx::InitializeOutput(FragmentTask* fragment_task, u64 task_id) {

}

SharedPtr<Table>
StreamFragmentCtx::GetResultInternal() {
    NotImplementError("StreamFragmentCtx::GetResultInternal")
}

}
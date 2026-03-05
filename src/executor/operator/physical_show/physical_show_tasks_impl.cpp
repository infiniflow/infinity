module infinity_core:physical_show_tasks.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :new_txn_manager;
import :bg_task;
import :query_context;
import :operator_state;
import :data_block;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowTasks(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    NewTxnManager *txn_mgr = query_context->storage()->new_txn_manager();
    std::vector<std::shared_ptr<BGTaskInfo>> bg_task_info_list = txn_mgr->GetTaskInfoList();
    // create data block for output state
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    output_block_ptr->Init(*output_types_);
    size_t row_count = 0;

    for (size_t i = 0; i < bg_task_info_list.size(); ++i) {
        BGTaskInfo *bg_task_info_ptr = bg_task_info_list[i].get();
        size_t task_count_ = bg_task_info_ptr->task_info_list_.size();
        for (size_t j = 0; j < task_count_; ++j) {
            if (!output_block_ptr) {
                output_block_ptr = DataBlock::MakeUniquePtr();
                output_block_ptr->Init(*output_types_);
            }

            size_t column_id = 0;
            {
                std::time_t task_time_t = std::chrono::system_clock::to_time_t(bg_task_info_ptr->task_time_);
                std::tm *task_tm = std::localtime(&task_time_t);
                std::ostringstream oss;
                oss << std::put_time(task_tm, "%Y-%m-%d %H:%M:%S");
                Value value = Value::MakeVarchar(oss.str());
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(ToString(bg_task_info_ptr->type_));
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                Value value = Value::MakeVarchar(bg_task_info_ptr->status_list_[j]);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            ++column_id;
            {
                std::string task_text = bg_task_info_ptr->task_info_list_[j];
                Value value = Value::MakeVarchar(task_text);
                ValueExpression value_expr(value);
                value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
            }

            if (++row_count == output_block_ptr->capacity()) {
                output_block_ptr->Finalize();
                show_operator_state->output_.emplace_back(std::move(output_block_ptr));
                output_block_ptr = nullptr;
                row_count = 0;
            }
        }
    }

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}
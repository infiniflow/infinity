module infinity_core:physical_show_list_snapshots.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :snapshot_brief;
import :utility;

import std;

using namespace infinity;

void PhysicalShow::ExecuteListSnapshots(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    output_block_ptr->Init(*output_types_);

    std::string snapshot_dir = query_context->global_config()->SnapshotDir();
    std::vector<SnapshotBrief> snapshot_list = SnapshotBrief::GetSnapshots(snapshot_dir);

    size_t row_count = 0;
    for (auto &snapshot_brief : snapshot_list) {
        if (output_block_ptr.get() == nullptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        {
            // snapshot name
            Value value = Value::MakeVarchar(snapshot_brief.snapshot_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[0]);
        }

        {
            // scope
            std::string scope_str;
            switch (snapshot_brief.scope_) {
                case SnapshotScope::kTable: {
                    scope_str = "Table";
                    break;
                }
                case SnapshotScope::kDatabase: {
                    scope_str = "Database";
                    break;
                }
                case SnapshotScope::kSystem: {
                    scope_str = "System";
                    break;
                }
                case SnapshotScope::kIgnore: {
                    scope_str = "Ignore";
                    break;
                }
                default: {
                    Status status = Status::Unknown("Invalid scope type");
                    RecoverableError(status);
                }
            }
            Value value = Value::MakeVarchar(scope_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[1]);
        }

        {
            // snapshot create time
            Value value = Value::MakeVarchar(snapshot_brief.create_time_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[2]);
        }

        {
            // snapshot commit ts
            Value value = Value::MakeBigInt(snapshot_brief.commit_ts_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[3]);
        }

        {
            // snapshot size
            std::string snapshot_size_str = Utility::FormatByteSize(snapshot_brief.size_);
            Value value = Value::MakeVarchar(snapshot_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[4]);
        }

        ++row_count;
        if (row_count == output_block_ptr->capacity()) {
            output_block_ptr->Finalize();
            operator_state->output_.emplace_back(std::move(output_block_ptr));
            output_block_ptr = nullptr;
            row_count = 0;
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}
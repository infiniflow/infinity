module infinity_core:physical_show_snapshot.impl;

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

void PhysicalShow::ExecuteShowSnapshot(QueryContext *query_context, ShowOperatorState *operator_state) {
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    output_block_ptr->Init(*output_types_);

    std::string snapshot_dir = query_context->global_config()->SnapshotDir();
    std::vector<SnapshotBrief> snapshot_list = SnapshotBrief::GetSnapshots(snapshot_dir);

    SnapshotBrief snapshot_brief;
    for (const auto &ss_brief : snapshot_list) {
        if (ss_brief.snapshot_name_ == object_name_.value()) {
            snapshot_brief = ss_brief;
        }
    }

    if (snapshot_brief.scope_ == SnapshotScope::kInvalid) {
        Status status = Status::Unknown(fmt::format("can't find snapshot: {}", object_name_.value()));
        RecoverableError(status);
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("snapshot_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(snapshot_brief.snapshot_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("snapshot_scope");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
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
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("create_time");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(snapshot_brief.create_time_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("commit_timestamp");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(snapshot_brief.commit_ts_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("snapshot_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            std::string snapshot_size_str = Utility::FormatByteSize(snapshot_brief.size_);
            Value value = Value::MakeVarchar(snapshot_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    operator_state->output_.emplace_back(std::move(output_block_ptr));
}
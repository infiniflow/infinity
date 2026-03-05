module infinity_core:physical_show_index_segment.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :infinity_context;
import :utility;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowIndexSegment(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Get tables from catalog
    std::shared_ptr<SegmentIndexInfo> segment_index_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(segment_index_info, status) = txn->GetSegmentIndexInfo(db_name_, *object_name_, index_name_.value(), segment_id_.value());
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    // Prepare the output data block
    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();

    output_block_ptr->Init(*output_types_);

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("segment_id");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_id_.value()));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    std::string full_segment_index_dir = std::filesystem::path(InfinityContext::instance().config()->DataDir()) / *segment_index_info->index_dir_;
    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_path");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(full_segment_index_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("index_segment_size");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            std::string index_size_str;
            if (query_context->persistence_manager() == nullptr) {
                index_size_str = Utility::FormatByteSize(VirtualStore::GetDirectorySize(full_segment_index_dir));
            } else {
                std::vector<std::string> &paths = segment_index_info->files_;
                size_t index_segment_size = 0;
                for (const std::string &path : paths) {
                    auto [file_size, status] = query_context->persistence_manager()->GetFileSize(path);
                    if (!status.ok()) {
                        RecoverableError(status);
                    }
                    index_segment_size += file_size;
                }
                index_size_str = Utility::FormatByteSize(index_segment_size);
            }

            Value value = Value::MakeVarchar(index_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("chunk_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(segment_index_info->chunk_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}
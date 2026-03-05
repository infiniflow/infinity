module infinity_core:physical_show_segments.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowSegments(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    std::vector<std::shared_ptr<SegmentInfo>> segment_info_list;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(segment_info_list, status) = txn->GetSegmentsInfo(db_name_, *object_name_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    size_t row_count = 0;
    output_block_ptr->Init(*output_types_);

    for (auto &segment_info : segment_info_list) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        size_t column_id = 0;
        {
            Value value = Value::MakeBigInt(segment_info->segment_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            //            SegmentEntry::SegmentStatusToString(segment_info->status_)
            Value value = Value::MakeVarchar("No value");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            std::string segment_size_str = "TODO";
            //            std::string full_segment_dir = fmt::format("{}/seg_{}")
            //            std::string full_segment_dir = std::filesystem::path(InfinityContext::instance().config()->DataDir()) /
            //            *segment_info->segment_dir_; if (query_context->persistence_manager() == nullptr) {
            //                segment_size_str = Utility::FormatByteSize(VirtualStore::GetDirectorySize(full_segment_dir));
            //            } else {
            //                const std::vector<std::string> &paths = segment_info->files_;
            //                size_t segment_size = 0;
            //                for (const std::string &path : paths) {
            //                    auto [file_size, status] = query_context->persistence_manager()->GetFileSize(path);
            //                    if (!status.ok()) {
            //                        RecoverableError(status);
            //                    }
            //                    segment_size += file_size;
            //                }
            //                segment_size_str = Utility::FormatByteSize(segment_size);
            //            }
            Value value = Value::MakeVarchar(segment_size_str);
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

    if (output_block_ptr) {
        output_block_ptr->Finalize();
        show_operator_state->output_.emplace_back(std::move(output_block_ptr));
    }
}
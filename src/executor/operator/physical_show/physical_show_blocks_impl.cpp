module infinity_core:physical_show_blocks.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;
import :data_block;
import :query_context;
import :operator_state;
import :infinity_context;
import :utility;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowBlocks(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    std::vector<std::shared_ptr<BlockInfo>> block_info_array;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(block_info_array, status) = txn->GetBlocksInfo(db_name_, *object_name_, *segment_id_);
    if (!status.ok()) {
        show_operator_state->status_ = status.clone();
        RecoverableError(status);
        return;
    }

    std::unique_ptr<DataBlock> output_block_ptr = DataBlock::MakeUniquePtr();
    size_t row_count = 0;
    output_block_ptr->Init(*output_types_);

    for (const auto &block_info : block_info_array) {
        if (!output_block_ptr) {
            output_block_ptr = DataBlock::MakeUniquePtr();
            output_block_ptr->Init(*output_types_);
        }

        size_t column_id = 0;
        {
            Value value = Value::MakeBigInt(block_info->block_id_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
        ++column_id;
        {
            size_t block_size = 0;
            std::vector<std::string> &paths = block_info->files_;
            bool check_buffer_obj = false;
            if (query_context->persistence_manager() == nullptr) {
                std::string full_block_dir = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *block_info->block_dir_);
                if (std::filesystem::exists(full_block_dir)) {
                    block_size = VirtualStore::GetDirectorySize(full_block_dir);
                } else {
                    check_buffer_obj = true;
                }
            } else {
                block_size = 0;
                for (const std::string &path : paths) {
                    auto [file_size, status2] = query_context->persistence_manager()->GetFileSize(path);
                    if (!status2.ok()) {
                        check_buffer_obj = true;
                        break;
                    }
                    block_size += file_size;
                }
            }

            // If block files are not found, try to get the buffer object from buffer manager.
            if (check_buffer_obj) {
                block_size = 0;
                BufferObj *buffer_obj = nullptr;
                BufferManager *buffer_manager = query_context->storage()->buffer_manager();

                for (const std::string &path : paths) {
                    std::string filepath = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), path);
                    buffer_obj = buffer_manager->GetBufferObject(filepath);
                    if (buffer_obj != nullptr) {
                        auto file_size = buffer_obj->GetBufferSize();
                        block_size += file_size;
                    } else {
                        RecoverableError(status);
                    }
                }
            }

            std::string block_size_str = Utility::FormatByteSize(block_size);
            Value value = Value::MakeVarchar(block_size_str);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeBigInt(block_info->row_count_);
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
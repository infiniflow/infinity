module infinity_core:physical_show_index.impl;

import :physical_show;
import :new_txn;
import :value;
import :value_expression;

import std;

using namespace infinity;

void PhysicalShow::ExecuteShowIndex(QueryContext *query_context, ShowOperatorState *show_operator_state) {
    // Get tables from catalog
    std::shared_ptr<TableIndexInfo> table_index_info;
    Status status;
    NewTxn *txn = query_context->GetNewTxn();
    std::tie(table_index_info, status) = txn->GetTableIndexInfo(db_name_, *object_name_, index_name_.value());
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
            Value value = Value::MakeVarchar("database_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(db_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("table_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*object_name_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("index_name");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(index_name_.value());
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }
    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("index_comment");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*(table_index_info->index_comment_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }
    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("index_type");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_type_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("index_column_names");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_column_names_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("index_column_ids");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_column_ids_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("function_info");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->function_info_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("other_parameters");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(*table_index_info->index_other_params_);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }
    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("storage_directory");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            const std::string *table_dir = table_index_info->index_entry_dir_.get();
            Value value = Value::MakeVarchar(*table_dir);
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }
    //    {
    //        size_t column_id = 0;
    //        {
    //            Value value = Value::MakeVarchar("storage_size");
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //
    //        ++column_id;
    //        {
    //            const std::string table_dir = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(),
    //            *table_index_info->index_entry_dir_); std::string index_size_str; if (query_context->persistence_manager() == nullptr) {
    //                index_size_str = Utility::FormatByteSize(VirtualStore::GetDirectorySize(table_dir));
    //            } else {
    //                const std::vector<std::string> &paths = table_index_info->files_;
    //                size_t index_size = 0;
    //                for (const std::string &path : paths) {
    //                    auto [file_size, status] = query_context->persistence_manager()->GetFileSize(path);
    //                    if (!status.ok()) {
    //                        RecoverableError(status);
    //                    }
    //                    index_size += file_size;
    //                }
    //                index_size_str = Utility::FormatByteSize(index_size);
    //            }
    //            Value value = Value::MakeVarchar(index_size_str);
    //            ValueExpression value_expr(value);
    //            value_expr.AppendToChunk(output_block_ptr->column_vectors[column_id]);
    //        }
    //    }

    {
        size_t column_id = 0;
        {
            Value value = Value::MakeVarchar("segment_index_count");
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }

        ++column_id;
        {
            Value value = Value::MakeVarchar(std::to_string(table_index_info->segment_index_count_));
            ValueExpression value_expr(value);
            value_expr.AppendToChunk(output_block_ptr->column_vectors_[column_id]);
        }
    }

    output_block_ptr->Finalize();
    show_operator_state->output_.emplace_back(std::move(output_block_ptr));
}
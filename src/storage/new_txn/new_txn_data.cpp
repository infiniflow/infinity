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

#include <string>

module new_txn;

import stl;
import third_party;
import new_txn_manager;
import kv_store;
import default_values;
import buffer_obj;
import infinity_exception;
import infinity_context;
import version_file_worker;
import block_version;
import buffer_handle;
import vector_buffer;
import logger;
import var_buffer;
import wal_entry;
import data_type;
import data_access_state;
import column_meta;
import block_meta;
import segment_meta;
import table_meeta;
import table_index_meeta;
import segment_index_meta;
import new_catalog;
import meta_key;
import db_meeta;
import build_fast_rough_filter_task;

import base_expression;
import cast_expression;
import value_expression;
import expression_binder;
import cast_function;
import bound_cast_func;
import constant_expr;
import expression_state;
import expression_evaluator;
import base_txn_store;
import catalog_cache;
import kv_code;
import bg_task;
import dump_index_process;
import mem_index;
import base_memindex;
import emvb_index_in_mem;
import txn_context;

namespace infinity {

struct NewTxnCompactState {
    NewTxnCompactState() = default;

    static Status Make(TableMeeta &table_meta, TxnTimeStamp commit_ts, NewTxnCompactState &state, SegmentID segment_id) {
        // Status status = NewCatalog::AddNewSegment1(table_meta, commit_ts, state.new_segment_meta_);
        Status status = NewCatalog::AddNewSegmentWithID(table_meta, commit_ts, state.new_segment_meta_, segment_id);
        if (!status.ok()) {
            return status;
        }
        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        state.column_cnt_ = column_defs_ptr->size();

        return Status::OK();
    };

    SizeT column_cnt() const { return column_cnt_; }

    Status NextBlock() {
        Status status;

        if (block_meta_) {
            // status = block_meta_->SetRowCnt(cur_block_row_cnt_);
            // if (!status.ok()) {
            //     return status;
            // }
            // segment_row_cnt_ += cur_block_row_cnt_;
            block_row_cnts_.push_back(cur_block_row_cnt_);
            segment_row_cnt_ += cur_block_row_cnt_;
            block_meta_.reset();
        }

        // BlockID block_id = 0;
        // {
        //     std::tie(block_id, status) = new_segment_meta_->GetNextBlockID();
        //     if (!status.ok()) {
        //         return status;
        //     }
        //     status = new_segment_meta_->SetNextBlockID(block_id + 1);
        //     if (!status.ok()) {
        //         return status;
        //     }
        // }
        status = NewCatalog::AddNewBlock1(*new_segment_meta_, commit_ts_, block_meta_);
        if (!status.ok()) {
            return status;
        }
        cur_block_row_cnt_ = 0;

        column_vectors_.clear();
        column_vectors_.resize(column_cnt_);

        for (SizeT i = 0; i < column_cnt_; ++i) {
            ColumnMeta column_meta(i, *block_meta_);
            status = NewCatalog::GetColumnVector(column_meta, 0, ColumnVectorTipe::kReadWrite, column_vectors_[i]);
            if (!status.ok()) {
                return status;
            }
        }
        return status;
    }

    Status FinalizeBlock() {
        if (block_meta_) {
            // Status status = block_meta_->SetRowCnt(cur_block_row_cnt_);
            // if (!status.ok()) {
            //     return status;
            // }
            block_row_cnts_.push_back(cur_block_row_cnt_);
            segment_row_cnt_ += cur_block_row_cnt_;
            for (ColumnID i = 0; i < column_cnt_; ++i) {
                ColumnMeta column_meta(i, *block_meta_);
                BufferObj *buffer_obj = nullptr;
                BufferObj *outline_buffer_obj = nullptr;

                Status status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
                if (!status.ok()) {
                    return status;
                }
                buffer_obj->Save();
                if (outline_buffer_obj) {
                    outline_buffer_obj->Save();
                }
            }
        }
        block_meta_.reset();
        cur_block_row_cnt_ = 0;
        return Status::OK();
    }

    Status Finalize() {
        Status status = FinalizeBlock();
        if (!status.ok()) {
            return status;
        }
        // status = new_segment_meta_->SetRowCnt(segment_row_cnt_);
        return status;
    }

    TxnTimeStamp commit_ts_{};

    Optional<SegmentMeta> new_segment_meta_{};
    Optional<BlockMeta> block_meta_{};

    Vector<SizeT> block_row_cnts_;
    SizeT segment_row_cnt_{};
    BlockOffset cur_block_row_cnt_{};
    Vector<ColumnVector> column_vectors_;
    SizeT column_cnt_{};
};

Status NewTxn::Import(const String &db_name, const String &table_name, const Vector<SharedPtr<DataBlock>> &input_blocks) {
    this->CheckTxn(db_name);

    Status status;
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    String table_key;
    status = GetTableMeta(db_name, table_name, db_meta, table_meta_opt, &table_key);
    if (!status.ok()) {
        return status;
    }

    TableMeeta &table_meta = *table_meta_opt;

    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    // status = NewCatalog::AddNewSegment1(table_meta, fake_commit_ts, segment_meta);
    u64 db_id = std::stoull(table_meta.db_id_str());
    u64 table_id = std::stoull(table_meta.table_id_str());
    SystemCache *system_cache = txn_mgr_->GetSystemCachePtr();
    Vector<SegmentID> segment_ids;

    SizeT input_block_count = input_blocks.size();
    SizeT segment_count = input_block_count % DEFAULT_BLOCK_PER_SEGMENT == 0
                        ? input_block_count / DEFAULT_BLOCK_PER_SEGMENT
                        : input_block_count / DEFAULT_BLOCK_PER_SEGMENT + 1;

    // If the number of input blocks is 0, infinity would output
    // "IMPORT 0 Rows" instead of throwing an exception.
    // It's debatable which is better. For now, we will keep the
    // behavior unchanged in order to avoid a breaking change to users.
    if (segment_count == 0) {
        segment_count = 1;
    }

    try {
        segment_ids = system_cache->ApplySegmentIDs(db_id, table_id, segment_count);
    } catch (const std::exception &e) {
        return Status::UnexpectedError(fmt::format("Database: {} or db is dropped: {}, cause: ", db_name, table_name, e.what()));
    }
    LOG_TRACE(fmt::format("Import: apply segment id starting at: {}, count {}", segment_ids[0], segment_count));

    Vector<UniquePtr<SegmentMeta>> segment_metas;
    segment_metas.reserve(segment_count);
    for (SizeT segment_idx = 0; segment_idx < segment_count; ++segment_idx) {
        Optional<SegmentMeta> segment_meta;
        status = NewCatalog::AddNewSegmentWithID(table_meta, fake_commit_ts, segment_meta, segment_ids[segment_idx]);
        if (!status.ok()) {
            return status;
        }
        segment_metas.emplace_back(MakeUnique<SegmentMeta>(segment_ids[segment_idx], table_meta));
    }

    Vector<SizeT> segment_row_cnts(segment_count, 0);
    Vector<Vector<SizeT>> block_row_cnts(segment_count);
    for (SizeT input_block_idx = 0; input_block_idx < input_blocks.size(); ++input_block_idx) {
        const SharedPtr<DataBlock> &input_block = input_blocks[input_block_idx];
        if (!input_block->Finalized()) {
            UnrecoverableError("Attempt to import unfinalized data block");
        }
        u32 row_cnt = input_block->row_count();

        Optional<BlockMeta> block_meta;
        // status = NewCatalog::AddNewBlock(*segment_meta, block_id, block_meta);
        SizeT segment_idx = input_block_idx / DEFAULT_BLOCK_PER_SEGMENT;
        status = NewCatalog::AddNewBlock1(*segment_metas[segment_idx], fake_commit_ts, block_meta);
        if (!status.ok()) {
            return status;
        }

        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }

        if (input_block_idx < input_blocks.size() - 1 && row_cnt != block_meta->block_capacity()) {
            UnrecoverableError("Attempt to import data block with different capacity");
        }

        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs{nullptr};
        {
            auto [col_defs, col_def_status] = table_meta.GetColumnDefs();
            if (!col_def_status.ok()) {
                return col_def_status;
            }
            column_defs = col_defs;
        }
        SizeT column_count = column_defs->size();
        if (column_count != input_block->column_count()) {
            String err_msg = fmt::format("Attempt to import different column count data block into transaction table store");
            LOG_ERROR(err_msg);
            return Status::ColumnCountMismatch(err_msg);
        }

        for (SizeT i = 0; i < input_block->column_count(); ++i) {
            SharedPtr<ColumnVector> col = input_block->column_vectors[i];

            ColumnMeta column_meta(i, *block_meta);

            BufferObj *buffer_obj = nullptr;
            BufferObj *outline_buffer_obj = nullptr;

            Status status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
            if (!status.ok()) {
                return status;
            }
            col->SetToCatalog(buffer_obj, outline_buffer_obj, ColumnVectorTipe::kReadWrite);
            // if (VarBufferManager *var_buffer_mgr = col->buffer_->var_buffer_mgr(); var_buffer_mgr != nullptr) {
            //     SizeT chunk_size = var_buffer_mgr->TotalSize();
            //     Status status = column_meta.SetChunkOffset(chunk_size);
            //     if (!status.ok()) {
            //         return status;
            //     }
            // }

            buffer_obj->Save();
            if (outline_buffer_obj) {
                outline_buffer_obj->Save();
            }
        }

        // status = block_meta->SetRowCnt(row_cnt);
        // if (!status.ok()) {
        //     return status;
        // }
        block_row_cnts[segment_idx].push_back(row_cnt);
        segment_row_cnts[segment_idx] += row_cnt;
    }
    // status = segment_meta->SetRowCnt(segment_row_cnt);
    // if (!status.ok()) {
    //     return status;
    // }
    Vector<WalSegmentInfo> segment_infos;
    segment_infos.reserve(segment_count);
    for (SizeT segment_idx = 0; segment_idx < segment_count; ++segment_idx) {
        WalSegmentInfo segment_info(*segment_metas[segment_idx], begin_ts);
        for (SizeT i = 0; i < block_row_cnts[segment_idx].size(); ++i) {
            segment_info.block_infos_[i].row_count_ = block_row_cnts[segment_idx][i];
        }
        segment_info.row_count_ = segment_row_cnts[segment_idx];
        status = this->AddSegmentVersion(segment_info, *segment_metas[segment_idx]);
        if (!status.ok()) {
            return status;
        }
        segment_infos.emplace_back(segment_info);
    }

    // index
    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_names_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ == nullptr) {
        base_txn_store_ = MakeShared<ImportTxnStore>();
        ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(base_txn_store_.get());
        import_txn_store->db_name_ = db_name;
        import_txn_store->db_id_str_ = table_meta.db_id_str();
        import_txn_store->table_name_ = table_name;
        import_txn_store->table_id_str_ = table_meta.table_id_str();
        import_txn_store->table_key_ = table_key;
        import_txn_store->table_id_ = std::stoull(table_meta.table_id_str());

        for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
            const String &index_id_str = (*index_id_strs_ptr)[i];
            const String &index_name = (*index_names_ptr)[i];
            import_txn_store->index_names_.emplace_back(index_name);
            import_txn_store->index_ids_str_.emplace_back(index_id_str);
            import_txn_store->index_ids_.emplace_back(std::stoull(index_id_str));
        }
    }
    ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(base_txn_store_.get());
    for (SizeT segment_idx = 0; segment_idx < segment_count; ++segment_idx) {
        SizeT input_block_start_idx = segment_idx * DEFAULT_BLOCK_PER_SEGMENT;
        SizeT input_block_end_idx = std::min(std::size_t(input_block_start_idx + DEFAULT_BLOCK_PER_SEGMENT), input_blocks.size());
        import_txn_store->input_blocks_in_imports_.emplace(
            segment_ids[segment_idx], Vector<SharedPtr<DataBlock>>(input_blocks.begin() + input_block_start_idx,
                                                                   input_blocks.begin() + input_block_end_idx));
    }
    import_txn_store->segment_infos_.insert(import_txn_store->segment_infos_.end(), segment_infos.begin(), segment_infos.end());
    import_txn_store->segment_ids_.insert(import_txn_store->segment_ids_.end(), segment_ids.begin(), segment_ids.end());

    for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
        const String &index_id_str = (*index_id_strs_ptr)[i];
        const String &index_name = (*index_names_ptr)[i];
        TableIndexMeeta table_index_meta(index_id_str, table_meta);

        for (SizeT segment_idx = 0; segment_idx < segment_count; ++segment_idx) {
            SizeT segment_row_cnt = segment_row_cnts[segment_idx];
            status = this->PopulateIndex(db_name,
                                         table_name,
                                         index_name,
                                         table_key,
                                         table_index_meta,
                                         *segment_metas[segment_idx],
                                         segment_row_cnt,
                                         DumpIndexCause::kImport);
            if (!status.ok()) {
                return status;
            }
        }
    }

    return Status::OK();
}

Status NewTxn::ReplayImport(WalCmdImportV2 *import_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    Status status = GetTableMeta(import_cmd->db_name_, import_cmd->table_name_, db_meta, table_meta_opt);
    if (!status.ok()) {
        return status;
    }
    TableMeeta &table_meta = *table_meta_opt;

    const WalSegmentInfo &segment_info = import_cmd->segment_info_;

    status = NewCatalog::LoadFlushedSegment2(table_meta, segment_info, fake_commit_ts);
    if (!status.ok()) {
        return status;
    }

    return PrepareCommitImport(import_cmd);
}

Status NewTxn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }
    base_txn_store_ = MakeShared<AppendTxnStore>();
    AppendTxnStore *append_txn_store = static_cast<AppendTxnStore *>(base_txn_store_.get());
    append_txn_store->db_name_ = db_name;
    append_txn_store->db_id_str_ = table_meta->db_id_str();
    append_txn_store->db_id_ = std::stoull(table_meta->db_id_str());
    append_txn_store->table_name_ = table_name;
    append_txn_store->table_id_str_ = table_meta->table_id_str();
    append_txn_store->table_id_ = std::stoull(table_meta->table_id_str());
    append_txn_store->input_block_ = input_block;
    // append_txn_store->row_ranges_ will be populated after conflict check

    return AppendInner(db_name, table_name, table_key, *table_meta, input_block);
}

Status NewTxn::Append(const TableInfo &table_info, const SharedPtr<DataBlock> &input_block) {
    return Append(*table_info.db_name_, *table_info.table_name_, input_block);
}

Status NewTxn::ReplayAppend(WalCmdAppendV2 *append_cmd, TxnTimeStamp commit_ts, i64 txn_id) { return Status::OK(); }

Status NewTxn::AppendInner(const String &db_name,
                           const String &table_name,
                           const String &table_key,
                           TableMeeta &table_meta,
                           const SharedPtr<DataBlock> &input_block) {

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs{nullptr};
    {
        auto [col_defs, col_def_status] = table_meta.GetColumnDefs();
        if (!col_def_status.ok()) {
            return col_def_status;
        }
        column_defs = col_defs;
    }
    SizeT column_count = column_defs->size();

    if (input_block->column_count() != column_count) {
        String err_msg = fmt::format("Attempt to insert different column count data block into transaction table store");
        LOG_ERROR(err_msg);

        return Status::ColumnCountMismatch(err_msg);
    }

    Vector<SharedPtr<DataType>> column_types;
    for (SizeT col_id = 0; col_id < column_count; ++col_id) {
        column_types.emplace_back((*column_defs)[col_id]->type());
        if (*column_types.back() != *input_block->column_vectors[col_id]->data_type()) {
            String err_msg = fmt::format("Attempt to insert different type data into transaction table store");
            LOG_ERROR(err_msg);

            return Status::DataTypeMismatch(column_types.back()->ToString(), input_block->column_vectors[col_id]->data_type()->ToString());
        }
    }

    return Status::OK();
}

Status NewTxn::Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids) {
    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta_opt, &table_key);
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ == nullptr) {
        base_txn_store_ = MakeShared<DeleteTxnStore>();
        DeleteTxnStore *delete_txn_store = static_cast<DeleteTxnStore *>(base_txn_store_.get());
        delete_txn_store->db_name_ = db_name;
        delete_txn_store->db_id_str_ = db_meta->db_id_str();
        delete_txn_store->db_id_ = std::stoull(db_meta->db_id_str());
        delete_txn_store->table_name_ = table_name;
        delete_txn_store->table_id_str_ = table_meta_opt->table_id_str();
        delete_txn_store->table_id_ = std::stoull(table_meta_opt->table_id_str());
        delete_txn_store->row_ids_ = row_ids;
    } else {
        DeleteTxnStore *delete_txn_store = static_cast<DeleteTxnStore *>(base_txn_store_.get());
        delete_txn_store->row_ids_.reserve(delete_txn_store->row_ids_.size() + row_ids.size());
        delete_txn_store->row_ids_.insert(delete_txn_store->row_ids_.end(), row_ids.begin(), row_ids.end());
    }

    return DeleteInner(db_name, table_name, *table_meta_opt, row_ids);
}

Status NewTxn::ReplayDelete(WalCmdDeleteV2 *delete_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(delete_cmd->db_name_, delete_cmd->table_name_, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    if (delete_cmd->db_id_ != db_meta->db_id_str() || delete_cmd->table_id_ != table_meta->table_id_str()) {
        return Status::CatalogError(fmt::format("WalCmdDeleteV2 db_id or table_id ({}, {}) mismatch with the expected value ({}, {})",
                                                delete_cmd->db_id_,
                                                delete_cmd->table_id_,
                                                db_meta->db_id_str(),
                                                table_meta->table_id_str()));
    }

    return PrepareCommitDelete(delete_cmd);
}

Status NewTxn::DeleteInner(const String &db_name, const String &table_name, TableMeeta &table_meta, const Vector<RowID> &row_ids) {
    auto delete_command = MakeShared<WalCmdDeleteV2>(db_name, table_meta.db_id_str(), table_name, table_meta.table_id_str(), row_ids);
    auto wal_command = static_pointer_cast<WalCmd>(delete_command);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(delete_command->ToString()));

    return Status::OK();
}

Status NewTxn::Update(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block, const Vector<RowID> &row_ids) {
    this->CheckTxn(db_name);

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, &table_key);
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ == nullptr) {
        base_txn_store_ = MakeShared<UpdateTxnStore>();
        UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(base_txn_store_.get());
        update_txn_store->db_name_ = db_name;
        update_txn_store->db_id_str_ = db_meta->db_id_str();
        update_txn_store->db_id_ = std::stoull(db_meta->db_id_str());
        update_txn_store->table_name_ = table_name;
        update_txn_store->table_id_str_ = table_meta->table_id_str();
        update_txn_store->table_id_ = std::stoull(table_meta->table_id_str());
        update_txn_store->input_blocks_.emplace_back(input_block);
        // update_txn_store->row_ranges_ will be populated after conflict check
        update_txn_store->row_ids_ = row_ids;
    } else {
        UpdateTxnStore *update_txn_store = static_cast<UpdateTxnStore *>(base_txn_store_.get());
        update_txn_store->input_blocks_.emplace_back(input_block);
        // append_txn_store->row_ranges_ will be populated after conflict check
        update_txn_store->row_ids_.reserve(update_txn_store->row_ids_.size() + row_ids.size());
        update_txn_store->row_ids_.insert(update_txn_store->row_ids_.end(), row_ids.begin(), row_ids.end());
    }

    status = AppendInner(db_name, table_name, table_key, *table_meta, input_block);
    if (!status.ok()) {
        return status;
    }
    status = this->DeleteInner(db_name, table_name, *table_meta, row_ids);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::Compact(const String &db_name, const String &table_name, const Vector<SegmentID> &segment_ids) {

    //    LOG_INFO(fmt::format("Start to compact segment ids: {}", segment_ids.size()));
    LOG_INFO(fmt::format("Compact db_name: {}, table_name: {}, segment ids: {}", db_name, table_name, fmt::join(segment_ids, " ")));

    this->SetTxnType(TransactionType::kCompact);

    this->CheckTxn(db_name);
    if (segment_ids.empty()) {
        return Status::UnexpectedError("No segment is given in compact operation");
    }

    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta_opt, &table_key);
    if (!status.ok()) {
        return status;
    }

    TableMeeta &table_meta = *table_meta_opt;
    status = table_meta.CheckSegments(segment_ids);
    if (!status.ok()) {
        return status;
    }

    // Fake commit timestamp, in prepare commit phase, it will be replaced by real commit timestamp
    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    SystemCache *system_cache = txn_mgr_->GetSystemCachePtr();
    u64 db_id = std::stoull(table_meta.db_id_str());
    u64 table_id = std::stoull(table_meta.table_id_str());
    Vector<SegmentID> new_segment_ids;
    try {
        new_segment_ids = system_cache->ApplySegmentIDs(db_id, table_id, 1);
    } catch (const std::exception &e) {
        return Status::UnexpectedError(fmt::format("Database: {} or db is dropped: {}, cause: ", db_name, table_name, e.what()));
    }
    LOG_TRACE(fmt::format("Compact: apply segment id: {}", new_segment_ids[0]));
    NewTxnCompactState compact_state;
    status = NewTxnCompactState::Make(table_meta, fake_commit_ts, compact_state, new_segment_ids[0]);
    if (!status.ok()) {
        return status;
    }

    LOG_TRACE(fmt::format("To compact segments {}", segment_ids.size()));
    for (SegmentID segment_id : segment_ids) {
        SegmentMeta segment_meta(segment_id, table_meta);

        Vector<BlockID> *block_ids_ptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return status;
        }

        for (BlockID block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);
            status = this->CompactBlock(block_meta, compact_state);
            if (!status.ok()) {
                return status;
            }
        }
        //        LOG_TRACE(fmt::format("Compact blocks of segment id: {}", segment_id));
    }
    status = compact_state.Finalize();
    if (!status.ok()) {
        return status;
    }

    {
        // Put the data into local txn store
        if (base_txn_store_ != nullptr) {
            return Status::UnexpectedError("txn store is not null");
        }
        Vector<WalSegmentInfo> segment_infos;
        segment_infos.emplace_back(*compact_state.new_segment_meta_, begin_ts);

        base_txn_store_ = MakeShared<CompactTxnStore>();
        CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(base_txn_store_.get());
        compact_txn_store->db_name_ = db_name;
        compact_txn_store->db_id_str_ = table_meta.db_id_str();
        compact_txn_store->db_id_ = std::stoull(table_meta.db_id_str());
        compact_txn_store->table_name_ = table_name;
        compact_txn_store->table_id_str_ = table_meta.table_id_str();
        compact_txn_store->table_id_ = std::stoull(table_meta.table_id_str());
        compact_txn_store->segment_infos_ = segment_infos;
        compact_txn_store->deprecated_segment_ids_ = segment_ids;
        compact_txn_store->new_segment_id_ = new_segment_ids[0];
        compact_txn_store->segment_ids_.emplace_back(new_segment_ids[0]);

        Vector<SegmentID> deprecated_segment_ids = segment_ids;
        {
            WalSegmentInfo &segment_info = segment_infos.back();
            segment_info.row_count_ = compact_state.segment_row_cnt_;
            if (segment_info.block_infos_.size() != compact_state.block_row_cnts_.size()) {
                UnrecoverableError(fmt::format("Block row count mismatch, expect: {}, actual: {}",
                                               segment_info.block_infos_.size(),
                                               compact_state.block_row_cnts_.size()));
            }
            for (SizeT i = 0; i < segment_info.block_infos_.size(); ++i) {
                WalBlockInfo &block_info = segment_info.block_infos_[i];
                block_info.row_count_ = compact_state.block_row_cnts_[i];
            }
        }
        auto compact_command = MakeShared<WalCmdCompactV2>(db_name,
                                                           db_meta->db_id_str(),
                                                           table_name,
                                                           table_meta.table_id_str(),
                                                           std::move(segment_infos),
                                                           std::move(deprecated_segment_ids));
        wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(compact_command));
        txn_context_ptr_->AddOperation(MakeShared<String>(compact_command->ToString()));

        status = this->AddSegmentVersion(compact_command->new_segment_infos_[0], *compact_state.new_segment_meta_);
        if (!status.ok()) {
            return status;
        }
    }

    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_name_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_name_ptr);
    if (!status.ok()) {
        return status;
    }

    CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(base_txn_store_.get());
    for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
        const String &index_id_str = (*index_id_strs_ptr)[i];
        const String &index_name = (*index_name_ptr)[i];
        compact_txn_store->index_names_.emplace_back(index_name);
        compact_txn_store->index_ids_str_.emplace_back(index_id_str);
        compact_txn_store->index_ids_.emplace_back(std::stoull(index_id_str));
    }

    //    LOG_TRACE(fmt::format("To populate index: {}", index_id_strs_ptr->size()));
    for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
        const String &index_id_str = (*index_id_strs_ptr)[i];
        const String &index_name = (*index_name_ptr)[i];
        TableIndexMeeta table_index_meta(index_id_str, table_meta);

        status = this->PopulateIndex(db_name,
                                     table_name,
                                     index_name,
                                     table_key,
                                     table_index_meta,
                                     *compact_state.new_segment_meta_,
                                     compact_state.segment_row_cnt_,
                                     DumpIndexCause::kCompact);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::CheckTableIfDelete(TableMeeta &table_meta, bool &has_delete) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    Status status = NewCatalog::CheckTableIfDelete(table_meta, begin_ts, has_delete);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewTxn::ReplayCompact(WalCmdCompactV2 *compact_cmd) {
    Status status;
    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    status = GetTableMeta(compact_cmd->db_name_, compact_cmd->table_name_, db_meta, table_meta_opt);
    if (!status.ok()) {
        return status;
    }
    TableMeeta &table_meta = *table_meta_opt;

    for (const WalSegmentInfo &segment_info : compact_cmd->new_segment_infos_) {
        status = NewCatalog::LoadFlushedSegment2(table_meta, segment_info, fake_commit_ts);
        if (!status.ok()) {
            return status;
        }
    }

    {
        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::ReplayCompact(WalCmdCompactV2 *compact_cmd, TxnTimeStamp commit_ts, i64 txn_id) {
    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    Status status = GetTableMeta(compact_cmd->db_name_, compact_cmd->table_name_, db_meta, table_meta_opt);
    if (!status.ok()) {
        return status;
    }
    TableMeeta &table_meta = *table_meta_opt;

    for (const WalSegmentInfo &segment_info : compact_cmd->new_segment_infos_) {
        status = NewCatalog::LoadFlushedSegment2(table_meta, segment_info, fake_commit_ts);
        if (!status.ok()) {
            return status;
        }
    }

    {
        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
        if (!status.ok()) {
            return status;
        }
        status = table_meta.RemoveSegmentIDs1(compact_cmd->deprecated_segment_ids_);
        if (!status.ok()) {
            return status;
        }
    }

    return PrepareCommitCompact(compact_cmd);
}

Status NewTxn::AppendInBlock(BlockMeta &block_meta, SizeT block_offset, SizeT append_rows, const DataBlock *input_block, SizeT input_offset) {
    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    auto [version_buffer, status] = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }

    SharedPtr<BlockLock> block_lock;
    {
        Status status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
    }
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    {
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        block_lock->min_ts_ = std::max(block_lock->min_ts_, commit_ts);
        block_lock->max_ts_ = std::max(block_lock->max_ts_, commit_ts);

        // append in column file
        for (SizeT column_idx = 0; column_idx < input_block->column_count(); ++column_idx) {
            const ColumnVector &column_vector = *input_block->column_vectors[column_idx];
            ColumnMeta column_meta(column_idx, block_meta);
            Status status = this->AppendInColumn(column_meta, block_offset, append_rows, column_vector, input_offset);
            if (!status.ok()) {
                return status;
            }
        }

        // append in version file.
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());
        block_version->Append(commit_ts, block_offset + append_rows);
    }
    return Status::OK();
}

// This function is called when block is locked
Status NewTxn::AppendInColumn(ColumnMeta &column_meta, SizeT dest_offset, SizeT append_rows, const ColumnVector &column_vector, SizeT source_offset) {
    ColumnVector dest_vec;
    {
        Status status = NewCatalog::GetColumnVector(column_meta, dest_offset, ColumnVectorTipe::kReadWrite, dest_vec);
        if (!status.ok()) {
            return status;
        }
    }
    dest_vec.AppendWith(column_vector, source_offset, append_rows);

    if (VarBufferManager *var_buffer_mgr = dest_vec.buffer_->var_buffer_mgr(); var_buffer_mgr != nullptr) {
        //     Ensure buffer obj is loaded.
        SizeT _ = var_buffer_mgr->TotalSize();
        //     Status status = column_meta.SetChunkOffset(chunk_size);
        //     if (!status.ok()) {
        //         return status;
        //     }
    }
    return Status::OK();
}

Status NewTxn::DeleteInBlock(BlockMeta &block_meta, const Vector<BlockOffset> &block_offsets, Vector<BlockOffset> &undo_block_offsets) {
    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    Status status;
    BufferObj *version_buffer = nullptr;
    std::tie(version_buffer, status) = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    {
        SharedPtr<BlockLock> block_lock;
        Status status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        // delete in version file
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());
        undo_block_offsets.reserve(block_offsets.size());
        for (BlockOffset block_offset : block_offsets) {
            Status status = block_version->Delete(block_offset, commit_ts);
            if (!status.ok()) {
                return status;
            }
            undo_block_offsets.push_back(block_offset);
        }
        block_lock->max_ts_ = std::max(block_lock->max_ts_, commit_ts); // FIXME: remove max_ts, undo delete should not revert max_ts
    }
    return Status::OK();
}

Status NewTxn::RollbackDeleteInBlock(BlockMeta &block_meta, const Vector<BlockOffset> &block_offsets) {
    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    BufferObj *version_buffer = nullptr;
    {
        String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + String(BlockVersion::PATH);
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        version_buffer = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer == nullptr) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
        }
    }

    {
        SharedPtr<BlockLock> block_lock;
        Status status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        // delete in version file
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());
        for (BlockOffset block_offset : block_offsets) {
            block_version->RollbackDelete(block_offset);
        }
    }
    return Status::OK();
}

Status NewTxn::PrintVersionInBlock(BlockMeta &block_meta, const Vector<BlockOffset> &block_offsets, bool ignore_invisible) {
    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    Status status;
    BufferObj *version_buffer = nullptr;
    std::tie(version_buffer, status) = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }

    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    {
        SharedPtr<BlockLock> block_lock;
        Status status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        // delete in version file
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());
        for (BlockOffset block_offset : block_offsets) {
            Status status = block_version->Print(begin_ts, block_offset, ignore_invisible);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status NewTxn::CompactBlock(BlockMeta &block_meta, NewTxnCompactState &compact_state) {
    Status status;

    NewTxnGetVisibleRangeState range_state;
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, range_state);
    if (!status.ok()) {
        return status;
    }

    SizeT block_row_cnt = range_state.block_offset_end();

    SizeT column_cnt = compact_state.column_cnt();
    Vector<ColumnVector> column_vectors;
    column_vectors.resize(column_cnt);
    for (SizeT column_id = 0; column_id < column_cnt; ++column_id) {
        ColumnMeta column_meta(column_id, block_meta);

        status = NewCatalog::GetColumnVector(column_meta, block_row_cnt, ColumnVectorTipe::kReadOnly, column_vectors[column_id]);
        if (!status.ok()) {
            return status;
        }
    }

    Pair<BlockOffset, BlockOffset> range;
    BlockOffset offset = 0;
    while (range_state.Next(offset, range)) {
        if (range.second == range.first) {
            offset = range.second;
            continue;
        }
        SizeT append_size = 0;
        while (true) {
            if (!compact_state.block_meta_) {
                status = compact_state.NextBlock();
                if (!status.ok()) {
                    return status;
                }
            }
            append_size = std::min(SizeT(range.second - range.first), compact_state.block_meta_->block_capacity() - compact_state.cur_block_row_cnt_);
            if (append_size == 0) {
                status = compact_state.FinalizeBlock();
                if (!status.ok()) {
                    return status;
                }
                status = compact_state.NextBlock();
                if (!status.ok()) {
                    return status;
                }

            } else {
                break;
            }
        }

        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
        TableMeeta &table_meta = block_meta.segment_meta().table_meta();
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }

        for (SizeT column_id = 0; column_id < column_cnt; ++column_id) {
            compact_state.column_vectors_[column_id].AppendWith(column_vectors[column_id], range.first, append_size);
        }
        compact_state.cur_block_row_cnt_ += append_size;
        offset = range.first + append_size;
    }

    return Status::OK();
}

Status NewTxn::AddColumnsData(TableMeeta &table_meta, const Vector<SharedPtr<ColumnDef>> &column_defs) {
    Status status;
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    if (segment_ids_ptr->empty()) {
        return Status::OK();
    }

    Vector<Value> default_values;
    ExpressionBinder tmp_binder(nullptr);
    for (const auto &column_def : column_defs) {
        if (!column_def->default_value()) {
            return Status::NotSupport(fmt::format("Column {} has no default value", column_def->name()));
        }
        SharedPtr<ConstantExpr> default_expr = column_def->default_value();
        auto expr = tmp_binder.BuildValueExpr(*default_expr, nullptr, 0, false);
        auto *value_expr = static_cast<ValueExpression *>(expr.get());

        const SharedPtr<DataType> &column_type = column_def->type();
        if (value_expr->Type() == *column_type) {
            default_values.push_back(value_expr->GetValue());
        } else {
            const SharedPtr<DataType> &column_type = column_def->type();

            BoundCastFunc cast = CastFunction::GetBoundFunc(value_expr->Type(), *column_type);
            SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(cast, expr, *column_type);
            SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(cast_expr);
            SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(column_type);
            output_column_vector->Initialize(ColumnVectorType::kConstant, 1);
            ExpressionEvaluator evaluator;
            evaluator.Init(nullptr);
            evaluator.Execute(cast_expr, expr_state, output_column_vector);

            default_values.push_back(output_column_vector->GetValue(0));
        }
    }

    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        status = this->AddColumnsDataInSegment(segment_meta, column_defs, default_values);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status
NewTxn::AddColumnsDataInSegment(SegmentMeta &segment_meta, const Vector<SharedPtr<ColumnDef>> &column_defs, const Vector<Value> &default_values) {
    auto [block_ids_ptr, status] = segment_meta.GetBlockIDs1();
    if (!status.ok()) {
        return status;
    }

    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta);
        status = this->AddColumnsDataInBlock(block_meta, column_defs, default_values);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::AddColumnsDataInBlock(BlockMeta &block_meta, const Vector<SharedPtr<ColumnDef>> &column_defs, const Vector<Value> &default_values) {
    // auto [block_row_count, status] = block_meta.GetRowCnt();
    auto [block_row_count, status] = block_meta.GetRowCnt1();
    if (!status.ok()) {
        return status;
    }
    SizeT old_column_cnt = 0;
    {
        auto [all_column_defs, status] = block_meta.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        old_column_cnt = all_column_defs->size() - column_defs.size();
    }
    for (SizeT i = 0; i < column_defs.size(); ++i) {
        SizeT column_idx = old_column_cnt + i;
        // const SharedPtr<ColumnDef> &column_def = column_defs[column_idx];
        const Value &default_value = default_values[i];

        Optional<ColumnMeta> column_meta;
        status = NewCatalog::AddNewBlockColumn(block_meta, column_idx, column_meta);
        if (!status.ok()) {
            return status;
        }

        ColumnVector column_vector;
        status = NewCatalog::GetColumnVector(*column_meta, 0 /*row_count*/, ColumnVectorTipe::kReadWrite, column_vector);
        if (!status.ok()) {
            return status;
        }

        for (SizeT i = 0; i < block_row_count; ++i) {
            column_vector.AppendValue(default_value);
        }
    }

    SharedPtr<BlockLock> block_lock;
    status = block_meta.GetBlockLock(block_lock);
    if (!status.ok()) {
        return status;
    }
    {
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);
        block_lock->max_ts_ = this->CommitTS();
    }

    return Status::OK();
}

Status NewTxn::DropColumnsData(TableMeeta &table_meta, const Vector<ColumnID> &column_ids) {
    Status status;
    Vector<SegmentID> *segment_ids_ptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    if (segment_ids_ptr->empty()) {
        return Status::OK();
    }

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
    if (!status.ok()) {
        return status;
    }

    auto drop_columns_in_block = [&](BlockMeta &block_meta) {
        TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

        for (ColumnID column_id : column_ids) {
            auto iter = std::find_if(column_defs_ptr->begin(), column_defs_ptr->end(), [&](const SharedPtr<ColumnDef> &column_def) {
                return ColumnID(column_def->id()) == column_id;
            });
            if (iter == column_defs_ptr->end()) {
                UnrecoverableError(fmt::format("Column {} not found in table meta", column_id));
            }
            SharedPtr<ColumnDef> column_def = *iter;

            auto ts_str = std::to_string(commit_ts);
            kv_instance_->Put(KeyEncode::DropBlockColumnKey(block_meta.segment_meta().table_meta().db_id_str(),
                                                            block_meta.segment_meta().table_meta().table_id_str(),
                                                            block_meta.segment_meta().segment_id(),
                                                            block_meta.block_id(),
                                                            column_def),
                              ts_str);
        }
        return Status::OK();
    };

    auto drop_columns_in_segment = [&](SegmentMeta &segment_meta) {
        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return status;
        }
        for (BlockID block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            status = drop_columns_in_block(block_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };

    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        status = drop_columns_in_segment(segment_meta);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CheckpointTable(TableMeeta &table_meta, const CheckpointOption &option, CheckpointTxnStore *ckp_txn_store) {
    Status status;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    Vector<BlockID> *block_ids_ptr = nullptr;
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return status;
        }
        for (BlockID block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            SharedPtr<BlockLock> block_lock;
            status = block_meta.GetBlockLock(block_lock);
            if (!status.ok()) {
                LOG_TRACE(fmt::format("NewTxn::CheckpointTable segment_id {}, block_id {}, got no BlockLock", segment_id, block_id));
                continue;
            }

            bool flush_version = false;
            bool flush_column = false;
            {
                // TODO: Refactor min_ts_ and max_ts_ to per-column-ts
                std::shared_lock<std::shared_mutex> lock(block_lock->mtx_);
                if (block_lock->checkpoint_ts_ < std::min(option.checkpoint_ts_, block_lock->max_ts_)) {
                    flush_version = true;
                }
                if (block_lock->checkpoint_ts_ < std::min(option.checkpoint_ts_, block_lock->max_ts_)) {
                    flush_column = true;
                }
            }
            if (flush_version) {
                status = FlushVersionFile(block_meta, option.checkpoint_ts_);
                if (!status.ok()) {
                    return status;
                }
            }
            if (flush_column) {
                status = FlushColumnFiles(block_meta, option.checkpoint_ts_);
                if (!status.ok()) {
                    return status;
                }
                bool to_mmap = false;
                status = TryToMmap(block_meta, option.checkpoint_ts_, &to_mmap);
                if (!status.ok()) {
                    return status;
                }
                if (to_mmap) {
                    LOG_INFO(fmt::format("Block {} to mmap, checkpoint ts: {}", block_meta.block_id(), option.checkpoint_ts_));
                }
            }

            if (!flush_column or !flush_version) {
                continue;
            } else {
                SharedPtr<FlushDataEntry> flush_data_entry =
                    MakeShared<FlushDataEntry>(table_meta.db_id_str(), table_meta.table_id_str(), segment_id, block_id);
                if (flush_column && flush_version) {
                    flush_data_entry->to_flush_ = "data and version";
                } else if (flush_column) {
                    flush_data_entry->to_flush_ = "data";
                } else {
                    flush_data_entry->to_flush_ = "version";
                }
                ckp_txn_store->entries_.emplace_back(flush_data_entry);
            }
        }
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitImport(WalCmdImportV2 *import_cmd) {
    Status status;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const String &db_id_str = import_cmd->db_id_;
    const String &table_id_str = import_cmd->table_id_;

    WalSegmentInfo &segment_info = import_cmd->segment_info_;
    TableMeeta table_meta(db_id_str, table_id_str, this);
    SegmentMeta segment_meta(segment_info.segment_id_, table_meta);

    status = table_meta.CommitSegment(segment_info.segment_id_, commit_ts);
    if (!status.ok()) {
        return status;
    }
    for (const WalBlockInfo &block_info : segment_info.block_infos_) {
        BlockID block_id = block_info.block_id_;
        status = segment_meta.CommitBlock(block_id, commit_ts);
        if (!status.ok()) {
            return status;
        }
    }

    status = this->CommitSegmentVersion(segment_info, segment_meta);
    if (!status.ok()) {
        return status;
    }

    BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(&segment_meta);

    return Status::OK();
}

Status NewTxn::CommitBottomAppend(WalCmdAppendV2 *append_cmd) {
    Status status;
    const String &db_name = append_cmd->db_name_;
    const String &table_name = append_cmd->table_name_;
    const String &db_id_str = append_cmd->db_id_;
    const String &table_id_str = append_cmd->table_id_;
    TxnTimeStamp commit_ts = CommitTS();
    TableMeeta table_meta(db_id_str, table_id_str, this);
    Optional<SegmentMeta> segment_meta;
    Optional<BlockMeta> block_meta;
    SizeT copied_row_cnt = 0;
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    Vector<TableIndexMeeta> table_index_metas;
    Vector<String> *index_name_strs = nullptr;
    if (!this->IsReplay()) {
        Vector<String> *index_id_strs = nullptr;
        {
            status = table_meta.GetIndexIDs(index_id_strs, &index_name_strs);
            if (!status.ok()) {
                return status;
            }
        }
        for (SizeT i = 0; i < index_id_strs->size(); ++i) {
            const String &index_id_str = (*index_id_strs)[i];
            table_index_metas.emplace_back(index_id_str, table_meta);
        }
    }

    // ensure append_cmd->row_ranges_ be block aligned
    Vector<Pair<RowID, u64>> append_ranges;
    auto calc_block_room = [](RowID row_id) -> u64 { return BLOCK_OFFSET_MASK - (row_id.segment_offset_ & BLOCK_OFFSET_MASK) + 1; };
    for (const Pair<RowID, u64> &range : append_cmd->row_ranges_) {
        RowID begin_row_id = range.first;
        u64 block_room = calc_block_room(begin_row_id);
        u64 left_rows = range.second;
        while (block_room < left_rows) {
            append_ranges.emplace_back(begin_row_id, block_room);
            left_rows -= block_room;
            begin_row_id.segment_offset_ += block_room;
            block_room = calc_block_room(begin_row_id);
        }
        if (left_rows > 0) {
            append_ranges.emplace_back(begin_row_id, left_rows);
        }
    }

    for (const Pair<RowID, u64> &range : append_ranges) {
        SegmentID segment_id = range.first.segment_id_;
        BlockID block_id = range.first.segment_offset_ >> BLOCK_OFFSET_SHIFT;
        u64 block_offset = range.first.segment_offset_ & BLOCK_OFFSET_MASK;
        if (range.first.segment_offset_ == 0) {
            // Create segment id in KV
            // table_meta.CommitSegment(segment_id, commit_ts);
            status = NewCatalog::AddNewSegmentWithID(table_meta, commit_ts, segment_meta, segment_id);
            if (!status.ok()) {
                return status;
            }
            if (segment_meta->segment_id() != segment_id) {
                UnrecoverableError(fmt::format("Segment id mismatch, expect: {}, actual: {}", segment_id, segment_meta->segment_id()));
            }
            table_meta.SetUnsealedSegmentID(segment_id);
            if (!status.ok()) {
                return status;
            }
        } else {
            segment_meta.emplace(segment_id, table_meta);
        }
        if (block_offset == 0) {
            // Create block id in KV
            // segment_meta->CommitBlock(block_id, commit_ts);
            // Create buffer objects for block and all columns
            status = NewCatalog::AddNewBlockWithID(*segment_meta, commit_ts, block_meta, block_id);
            if (!status.ok()) {
                return status;
            }
            if (block_meta->block_id() != block_id) {
                UnrecoverableError(fmt::format("Block id mismatch, expect: {}, actual: {}", block_id, block_meta->block_id()));
            }
        } else {
            block_meta.emplace(block_id, segment_meta.value());
        }
        SizeT block_row_cnt;
        std::tie(block_row_cnt, status) = block_meta->GetRowCnt1();
        if (!status.ok()) {
            return status;
        }
        LOG_DEBUG(fmt::format("CommitBottomAppend block {}, existing row cnt {}, new row cnt {}", block_id, block_row_cnt, range.second));

        status = this->AppendInBlock(*block_meta, block_offset, range.second, append_cmd->block_.get(), copied_row_cnt);
        if (!status.ok()) {
            return status;
        }
        for (auto &table_index_meta : table_index_metas) {
            status = this->AppendIndex(table_index_meta, range);
            if (!status.ok()) {
                return status;
            }
        }

        // If reach the end of segment, set it to sealed and build fast rough filter
        if (range.first.segment_offset_ + range.second == DEFAULT_SEGMENT_CAPACITY) {
            table_meta.DelUnsealedSegmentID();
            BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(&segment_meta.value());

            for (SizeT i = 0; i < table_index_metas.size(); ++i) {
                const String &index_name = (*index_name_strs)[i];
                SharedPtr<DumpMemIndexTask> dump_index_task = MakeShared<DumpMemIndexTask>(db_name, table_name, index_name, segment_id);
                // Trigger dump index processor to dump mem index for new sealed segment
                auto *dump_index_processor = InfinityContext::instance().storage()->dump_index_processor();
                dump_index_processor->Submit(dump_index_task);
            }
        }
        copied_row_cnt += range.second;
    }
    return Status::OK();
}

Status NewTxn::PrepareCommitDelete(const WalCmdDeleteV2 *delete_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const String &db_id_str = delete_cmd->db_id_;
    const String &table_id_str = delete_cmd->table_id_;

    TableMeeta table_meta(db_id_str, table_id_str, this);

    Optional<SegmentMeta> segment_meta;
    Optional<BlockMeta> block_meta;

    NewTxnTableStore1 *txn_table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    Status delete_status = txn_table_store->Delete(delete_cmd->row_ids_);
    if (!delete_status.ok()) {
        return delete_status;
    }
    DeleteState &delete_state = txn_table_store->delete_state();
    DeleteState &undo_delete_state = txn_table_store->undo_delete_state();
    for (const auto &[segment_id, block_map] : delete_state.rows_) {
        if (!segment_meta || segment_id != segment_meta->segment_id()) {
            segment_meta.emplace(segment_id, table_meta);
            block_meta.reset();
        }
        auto &undo_segment_map = undo_delete_state.rows_[segment_id];
        for (const auto &[block_id, block_offsets] : block_map) {
            if (!block_meta || block_id != block_meta->block_id()) {
                block_meta.emplace(block_id, segment_meta.value());
            }
            auto &undo_block_offsets = undo_segment_map[block_id];
            Status status = this->DeleteInBlock(*block_meta, block_offsets, undo_block_offsets);
            if (!status.ok()) {
                return status;
            }
        }

        {
            TxnTimeStamp first_delete_ts = 0;
            Status status = segment_meta->GetFirstDeleteTS(first_delete_ts);
            if (!status.ok()) {
                return status;
            }
            if (first_delete_ts == UNCOMMIT_TS) {
                status = segment_meta->SetFirstDeleteTS(commit_ts);
                if (!status.ok()) {
                    return status;
                }
            }
        }
    }
    delete_state.rows_.clear();
    return Status::OK();
}

Status NewTxn::RollbackDelete(const DeleteTxnStore *delete_txn_store) {
    const String &db_id_str = delete_txn_store->db_id_str_;
    const String &table_id_str = delete_txn_store->table_id_str_;

    TableMeeta table_meta(db_id_str, table_id_str, this);

    Optional<SegmentMeta> segment_meta;
    Optional<BlockMeta> block_meta;

    // Get undo delete state
    NewTxnTableStore1 *txn_table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    DeleteState &undo_delete_state = txn_table_store->undo_delete_state();

    for (const auto &[segment_id, block_map] : undo_delete_state.rows_) {
        if (!segment_meta || segment_id != segment_meta->segment_id()) {
            segment_meta.emplace(segment_id, table_meta);
            block_meta.reset();
        }
        auto &undo_block_map = undo_delete_state.rows_[segment_id];
        for (const auto &[block_id, block_offsets] : undo_block_map) {
            if (!block_meta || block_id != block_meta->block_id()) {
                block_meta.emplace(block_id, segment_meta.value());
            }
            Status status = this->RollbackDeleteInBlock(*block_meta, block_offsets);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status NewTxn::PrepareCommitCompact(WalCmdCompactV2 *compact_cmd) {
    Status status;
    const String &db_id_str = compact_cmd->db_id_;
    const String &table_id_str = compact_cmd->table_id_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    Vector<WalSegmentInfo> &segment_infos = compact_cmd->new_segment_infos_;
    if (segment_infos.empty()) {
        return Status::OK();
    }
    if (segment_infos.size() > 1) {
        UnrecoverableError("Not implemented");
    }
    WalSegmentInfo &segment_info = segment_infos[0];
    Vector<SegmentID> new_segment_ids{segment_info.segment_id_};

    TableMeeta table_meta(db_id_str, table_id_str, this);
    SegmentMeta segment_meta(segment_info.segment_id_, table_meta);

    status = table_meta.CommitSegment(segment_info.segment_id_, commit_ts);
    if (!status.ok()) {
        return status;
    }
    for (const WalBlockInfo &block_info : segment_info.block_infos_) {
        BlockID block_id = block_info.block_id_;
        status = segment_meta.CommitBlock(block_id, commit_ts);
        if (!status.ok()) {
            return status;
        }
    }

    status = this->CommitSegmentVersion(segment_info, segment_meta);
    if (!status.ok()) {
        return status;
    }

    BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(&segment_meta);

    const Vector<SegmentID> &deprecated_ids = compact_cmd->deprecated_segment_ids_;

    for (SegmentID segment_id : deprecated_ids) {
        auto ts_str = std::to_string(commit_ts);
        kv_instance_->Put(KeyEncode::DropSegmentKey(db_id_str, table_id_str, segment_id), ts_str);
    }
    {
        Vector<String> *index_id_strs_ptr = nullptr;
        status = table_meta.GetIndexIDs(index_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &index_id_str : *index_id_strs_ptr) {
            TableIndexMeeta table_index_meta(index_id_str, table_meta);
            Vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_index_meta.GetSegmentIndexIDs1();
            if (!status.ok()) {
                return status;
            }
            for (SegmentID segment_id : *segment_ids_ptr) {
                auto iter = std::find(deprecated_ids.begin(), deprecated_ids.end(), segment_id);
                if (iter != deprecated_ids.end()) {
                    auto ts_str = std::to_string(commit_ts);
                    kv_instance_->Put(KeyEncode::DropSegmentIndexKey(db_id_str, table_id_str, index_id_str, segment_id), ts_str);
                }
            }
            status = table_index_meta.RemoveSegmentIndexIDs(deprecated_ids);
            if (!status.ok()) {
                return status;
            }
            //  status = table_index_meta.SetSegmentIDs(new_segment_ids);
            //  if (!status.ok()) {
            //      return status;
            //  }
        }
    }
    return Status::OK();
}

Status NewTxn::CommitCheckpointTableData(TableMeeta &table_meta, TxnTimeStamp checkpoint_ts) {
    Status status;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        auto [block_ids, status] = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return status;
        }
        for (BlockID block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);

            SharedPtr<BlockLock> block_lock;
            status = block_meta.GetBlockLock(block_lock);
            if (!status.ok()) {
                return status;
            }

            {
                std::shared_lock<std::shared_mutex> lock(block_lock->mtx_);
                block_lock->checkpoint_ts_ = checkpoint_ts;
            }
        }
    }

    return Status::OK();
}

Status NewTxn::AddSegmentVersion(WalSegmentInfo &segment_info, SegmentMeta &segment_meta) {
    TxnTimeStamp save_ts = txn_context_ptr_->begin_ts_;
    for (WalBlockInfo &block_info : segment_info.block_infos_) {
        BlockMeta block_meta(block_info.block_id_, segment_meta);
        SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();

        auto [version_buffer, status] = block_meta.GetVersionBuffer();
        if (!status.ok()) {
            return status;
        }
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());

        block_version->Append(save_ts, block_info.row_count_);
    }
    return Status::OK();
}

Status NewTxn::CommitSegmentVersion(WalSegmentInfo &segment_info, SegmentMeta &segment_meta) {
    TxnTimeStamp save_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    auto *pm = InfinityContext::instance().persistence_manager();

    for (WalBlockInfo &block_info : segment_info.block_infos_) {
        BlockMeta block_meta(block_info.block_id_, segment_meta);
        SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();

        auto [version_buffer, status] = block_meta.GetVersionBuffer();
        if (!status.ok()) {
            return status;
        }
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());

        block_version->CommitAppend(save_ts, commit_ts);
        version_buffer->Save(VersionFileWorkerSaveCtx(commit_ts));

        SharedPtr<BlockLock> block_lock;
        status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
        {
            std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);
            block_lock->min_ts_ = std::max(block_lock->min_ts_, commit_ts);
            block_lock->max_ts_ = std::max(block_lock->max_ts_, commit_ts);
            block_lock->checkpoint_ts_ = commit_ts;
        }

        if (pm) {
            block_info.addr_serializer_.InitializeValid(pm);
        }
    }

    return Status::OK();
}

Status NewTxn::FlushVersionFile(BlockMeta &block_meta, TxnTimeStamp save_ts) {
    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    BufferObj *version_buffer = nullptr;
    {
        String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + String(BlockVersion::PATH);
        version_buffer = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer == nullptr) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
        }
    }

    version_buffer->Save(VersionFileWorkerSaveCtx(save_ts));
    return Status::OK();
}

Status NewTxn::FlushColumnFiles(BlockMeta &block_meta, TxnTimeStamp save_ts) {
    Status status;

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    for (SizeT column_idx = 0; column_idx < column_defs->size(); ++column_idx) {
        ColumnMeta column_meta(column_idx, block_meta);
        BufferObj *buffer_obj = nullptr;
        BufferObj *outline_buffer_obj = nullptr;

        status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
        if (!status.ok()) {
            return status;
        }
        buffer_obj->Save();
        if (outline_buffer_obj) {
            outline_buffer_obj->Save();
        }
    }
    return Status::OK();
}

Status NewTxn::TryToMmap(BlockMeta &block_meta, TxnTimeStamp save_ts, bool *to_mmap_ptr) {
    Status status;

    SizeT row_cnt;
    std::tie(row_cnt, status) = block_meta.GetRowCnt1();
    if (!status.ok()) {
        return status;
    }
    bool to_mmap = row_cnt >= block_meta.block_capacity();
    if (to_mmap_ptr) {
        *to_mmap_ptr = to_mmap;
    }
    if (to_mmap) {
        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
        std::tie(column_defs, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        for (SizeT column_idx = 0; column_idx < column_defs->size(); ++column_idx) {
            ColumnMeta column_meta(column_idx, block_meta);
            BufferObj *buffer_obj = nullptr;
            BufferObj *outline_buffer_obj = nullptr;

            Status status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
            if (!status.ok()) {
                return status;
            }
            buffer_obj->ToMmap();
            if (outline_buffer_obj) {
                outline_buffer_obj->ToMmap();
            }
        }
    }
    return Status::OK();
}

} // namespace infinity

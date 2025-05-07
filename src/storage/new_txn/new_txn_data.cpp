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

namespace infinity {

struct NewTxnCompactState {
    NewTxnCompactState() = default;

    static Status Make(TableMeeta &table_meta, TxnTimeStamp commit_ts, NewTxnCompactState &state) {
        Status status = NewCatalog::AddNewSegment1(table_meta, commit_ts, state.new_segment_meta_);
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

    TxnTimeStamp commit_ts_;
    Optional<SegmentMeta> new_segment_meta_;
    Optional<BlockMeta> block_meta_;

    Vector<SizeT> block_row_cnts_;
    SizeT segment_row_cnt_ = 0;
    BlockOffset cur_block_row_cnt_ = 0;
    Vector<ColumnVector> column_vectors_;
    SizeT column_cnt_ = 0;
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

    status = this->IncreaseTableReferenceCount(table_key);
    if (!status.ok()) {
        return status;
    }

    TableMeeta &table_meta = *table_meta_opt;

    // SegmentID segment_id = 0;
    // status = table_meta.GetNextSegmentID(segment_id);
    // if (!status.ok()) {
    //     return status;
    // }
    // status = table_meta.SetNextSegmentID(segment_id + 1);
    // if (!status.ok()) {
    //     return status;
    // }
    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    Optional<SegmentMeta> segment_meta;
    // status = NewCatalog::AddNewSegment(table_meta, segment_id, segment_meta);
    status = NewCatalog::AddNewSegment1(table_meta, fake_commit_ts, segment_meta);
    if (!status.ok()) {
        return status;
    }

    SizeT segment_row_cnt = 0;
    Vector<SizeT> block_row_cnts;
    for (SizeT j = 0; j < input_blocks.size(); ++j) {
        const SharedPtr<DataBlock> &input_block = input_blocks[j];
        if (!input_block->Finalized()) {
            UnrecoverableError("Attempt to import unfinalized data block");
        }
        u32 row_cnt = input_block->row_count();

        // BlockID block_id = 0;
        // {
        //     std::tie(block_id, status) = segment_meta->GetNextBlockID();
        //     if (!status.ok()) {
        //         return status;
        //     }
        //     status = segment_meta->SetNextBlockID(block_id + 1);
        //     if (!status.ok()) {
        //         return status;
        //     }
        // }
        Optional<BlockMeta> block_meta;
        // status = NewCatalog::AddNewBlock(*segment_meta, block_id, block_meta);
        status = NewCatalog::AddNewBlock1(*segment_meta, fake_commit_ts, block_meta);
        if (!status.ok()) {
            return status;
        }

        if (j < input_blocks.size() - 1 && row_cnt != block_meta->block_capacity()) {
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
            if (VarBufferManager *var_buffer_mgr = col->buffer_->var_buffer_mgr(); var_buffer_mgr != nullptr) {
                SizeT chunk_size = var_buffer_mgr->TotalSize();
                Status status = column_meta.SetChunkOffset(chunk_size);
                if (!status.ok()) {
                    return status;
                }
            }

            buffer_obj->Save();
            if (outline_buffer_obj) {
                outline_buffer_obj->Save();
            }
        }

        // status = block_meta->SetRowCnt(row_cnt);
        // if (!status.ok()) {
        //     return status;
        // }
        block_row_cnts.push_back(row_cnt);
        segment_row_cnt += row_cnt;
    }
    // status = segment_meta->SetRowCnt(segment_row_cnt);
    // if (!status.ok()) {
    //     return status;
    // }

    { // Add import wal;
        auto import_command =
            MakeShared<WalCmdImportV2>(db_name, db_meta->db_id_str(), table_name, table_meta.table_id_str(), WalSegmentInfo(*segment_meta, begin_ts));
        if (block_row_cnts.size() != import_command->segment_info_.block_infos_.size()) {
            UnrecoverableError("Block row count mismatch");
        }
        for (SizeT i = 0; i < block_row_cnts.size(); ++i) {
            import_command->segment_info_.block_infos_[i].row_count_ = block_row_cnts[i];
        }
        import_command->segment_info_.row_count_ = segment_row_cnt;

        wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(import_command));
        txn_context_ptr_->AddOperation(MakeShared<String>(import_command->ToString()));

        status = this->AddSegmentVersion(import_command->segment_info_, *segment_meta);
        if (!status.ok()) {
            return status;
        }
    }

    // index
    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_names_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
        const String &index_id_str = (*index_id_strs_ptr)[i];
        const String &index_name = (*index_names_ptr)[i];
        TableIndexMeeta table_index_meta(index_id_str, table_meta);

        status = this->PopulateIndex(db_name,
                                     table_name,
                                     index_name,
                                     table_key,
                                     table_index_meta,
                                     *segment_meta,
                                     segment_row_cnt,
                                     DumpIndexCause::kImport);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::ReplayImport(WalCmdImportV2 *import_cmd) {
    Status status;
    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    status = GetTableMeta(import_cmd->db_name_, import_cmd->table_name_, db_meta, table_meta_opt);
    if (!status.ok()) {
        return status;
    }
    TableMeeta &table_meta = *table_meta_opt;

    const WalSegmentInfo &segment_info = import_cmd->segment_info_;

    status = NewCatalog::LoadFlushedSegment1(table_meta, segment_info, fake_commit_ts);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
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

    status = this->IncreaseMemIndexReferenceCount(table_key);
    if (!status.ok()) {
        return status;
    }

    status = this->IncreaseTableReferenceCount(table_key);
    if (!status.ok()) {
        return status;
    }

    return AppendInner(db_name, table_name, table_key, *table_meta, input_block);
}

Status NewTxn::Append(const TableInfo &table_info, const SharedPtr<DataBlock> &input_block) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TableMeeta table_meta(table_info.db_id_, table_info.table_id_, *kv_instance_, begin_ts);
    return AppendInner(*table_info.db_name_, *table_info.table_name_, table_info.table_key_, table_meta, input_block);
}

Status NewTxn::AppendInner(const String &db_name,
                           const String &table_name,
                           const String &table_key,
                           TableMeeta &table_meta,
                           const SharedPtr<DataBlock> &input_block) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    Vector<Pair<RowID, u64>> row_ranges;

    // Read col definition in new rocksdb transaction
    KVStore *kv_store = txn_mgr_->kv_store();
    UniquePtr<KVInstance> kv_instance_validation = kv_store->GetInstance();
    TableMeeta table_meta_validation(table_meta.db_id_str(), table_meta.table_id_str(), *kv_instance_validation, begin_ts);
    auto [column_defs_validation, column_defs_validation_status] = table_meta_validation.GetColumnDefs();
    if (!column_defs_validation_status.ok()) {
        return column_defs_validation_status;
    }

    {
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

        if (column_defs_validation->size() != column_count) {
            String err_msg = fmt::format("Insert data conflicts with alter columns, expected column count: {}, actual column count: {}",
                                         column_count,
                                         column_defs_validation->size());
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
            if (*column_types.back() != *(*column_defs_validation)[col_id]->type()) {
                String err_msg = fmt::format("Insert data conflicts with alter column: column_id: {}, left_type:{}, right_type{}",
                                             col_id,
                                             column_types.back()->ToString(),
                                             (*column_defs_validation)[col_id]->type()->ToString());
                LOG_ERROR(err_msg);

                return Status::DataTypeMismatch(column_types.back()->ToString(), (*column_defs_validation)[col_id]->type()->ToString());
            }
        }
    }

    // Put the data into local txn store
    base_txn_store_ = MakeShared<AppendTxnStore>();
    AppendTxnStore *append_txn_store = static_cast<AppendTxnStore *>(base_txn_store_.get());
    append_txn_store->db_name_ = db_name;
    append_txn_store->db_id_str_ = table_meta.db_id_str();
    append_txn_store->db_id_ = std::stoull(table_meta.db_id_str());
    append_txn_store->table_name_ = table_name;
    append_txn_store->table_id_str_ = table_meta.table_id_str();
    append_txn_store->table_id_ = std::stoull(table_meta.table_id_str());
    append_txn_store->input_block_ = input_block;

    auto append_command = MakeShared<WalCmdAppendV2>(db_name, table_meta.db_id_str(), table_name, table_meta.table_id_str(), row_ranges, input_block);
    RowID begin_row_id;
    Status status = table_meta.GetNextRowID(begin_row_id);
    if (!status.ok()) {
        return status;
    }
    SizeT left_rows = append_command->block_->row_count();
    while (left_rows > 0) {
        SegmentID segment_id = begin_row_id.segment_id_;
        SizeT segment_row_cnt = begin_row_id.segment_offset_;
        SizeT segment_room = DEFAULT_SEGMENT_CAPACITY - segment_row_cnt;
        SizeT copyed_rows = left_rows < segment_room ? left_rows : segment_room;
        append_command->row_ranges_.push_back({begin_row_id, copyed_rows});
        left_rows -= copyed_rows;
        begin_row_id = RowID(segment_id + 1, 0);
    }

    auto wal_command = static_pointer_cast<WalCmd>(append_command);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(append_command->ToString()));
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

    status = this->IncreaseTableReferenceCount(table_key);
    if (!status.ok()) {
        return status;
    }

    auto delete_command = MakeShared<WalCmdDeleteV2>(db_name, db_meta->db_id_str(), table_name, table_meta_opt->table_id_str(), row_ids);
    auto wal_command = static_pointer_cast<WalCmd>(delete_command);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(delete_command->ToString()));

    return Status::OK();
}

Status NewTxn::PrintVersion(const String &db_name, const String &table_name, const Vector<RowID> &row_ids, bool ignore_invisible) {

    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta_opt;
    String table_key;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta_opt, &table_key);
    if (!status.ok()) {
        return status;
    }

    const String &db_id_str = db_meta->db_id_str();
    const String &table_id_str = table_meta_opt->table_id_str();

    TableMeeta &table_meta = *table_meta_opt;

    Optional<SegmentMeta> segment_meta;
    Optional<BlockMeta> block_meta;

    NewTxnTableStore1 *txn_table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    AccessState access_state;
    txn_table_store->GetAccessState(row_ids, access_state);
    for (const auto &[segment_id, block_map] : access_state.rows_) {
        if (!segment_meta || segment_id != segment_meta->segment_id()) {
            segment_meta.emplace(segment_id, table_meta);
            block_meta.reset();
        }
        LOG_INFO(fmt::format("Segment {} version: ", segment_id));
        for (const auto &[block_id, block_offsets] : block_map) {
            if (!block_meta || block_id != block_meta->block_id()) {
                block_meta.emplace(block_id, segment_meta.value());
            }
            LOG_INFO(fmt::format("Block {} version: ", block_id));
            Status status = this->PrintVersionInBlock(*block_meta, block_offsets, ignore_invisible);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status NewTxn::Compact(const String &db_name, const String &table_name, const Vector<SegmentID> &segment_ids) {

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

    status = this->IncreaseTableReferenceCount(table_key);
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

    NewTxnCompactState compact_state;
    status = NewTxnCompactState::Make(table_meta, fake_commit_ts, compact_state);
    if (!status.ok()) {
        return status;
    }

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
    }
    status = compact_state.Finalize();
    if (!status.ok()) {
        return status;
    }

    status = table_meta.RemoveSegmentIDs1(segment_ids);
    if (!status.ok()) {
        return status;
    }
    {
        Vector<SegmentID> deprecated_segment_ids = segment_ids;
        Vector<WalSegmentInfo> segment_infos;
        segment_infos.emplace_back(*compact_state.new_segment_meta_, begin_ts);
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

Status NewTxn::CheckTableIfDelete(const String &db_name, const String &table_name, bool &has_delete) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    Optional<DBMeeta> db_meta;
    Optional<TableMeeta> table_meta;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta);
    if (!status.ok()) {
        return status;
    }
    status = NewCatalog::CheckTableIfDelete(*table_meta, begin_ts, has_delete);
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
        status = NewCatalog::LoadFlushedSegment1(table_meta, segment_info, fake_commit_ts);
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

    return Status::OK();
}

Status NewTxn::PrepareAppendInBlock(BlockMeta &block_meta, AppendState *append_state, bool &block_full, bool &segment_full) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;

    auto [segment_row_count, segment_status] = block_meta.segment_meta().GetRowCnt1();
    if (!segment_status.ok()) {
        return segment_status;
    }

    while (append_state->current_block_ < append_state->blocks_.size()) {
        DataBlock *input_block = append_state->blocks_[append_state->current_block_].get();
        SizeT to_append_rows = input_block->row_count() - append_state->current_block_offset_;
        if (to_append_rows == 0) {
            ++append_state->current_block_;
            append_state->current_block_offset_ = 0;
            continue;
        }

        auto [version_buffer, status] = block_meta.GetVersionBuffer();
        if (!status.ok()) {
            return status;
        }
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());

        // auto [block_row_count, block_status] = block_meta.GetRowCnt();
        // auto [block_row_count, block_status] = block_meta.GetRowCnt1(begin_ts);
        auto [block_row_count, block_status] = block_version->GetRowCountForUpdate(begin_ts);
        if (!block_status.ok()) {
            return block_status;
        }

        SizeT actual_append = std::min(to_append_rows, block_meta.block_capacity() - block_row_count);
        if (actual_append) {
            SegmentID segment_id = block_meta.segment_meta().segment_id();
            BlockID block_id = block_meta.block_id();
            AppendRange range(segment_id,
                              block_id,
                              block_row_count,
                              actual_append,
                              append_state->current_block_,
                              append_state->current_block_offset_);
            append_state->append_ranges_.push_back(range);
            //            LOG_INFO(fmt::format("actual to append rows: {}, from block: {} and offset {} to target block id: {} and offset: {}",
            //                                 to_append_rows,
            //                                 append_state->current_block_,
            //                                 append_state->current_block_offset_,
            //                                 block_id,
            //                                 block_row_count));
            // Status status = block_meta.SetRowCnt(block_row_count + actual_append);
            // if (!status.ok()) {
            //     return status;
            // }
        }

        block_full = block_row_count + actual_append >= block_meta.block_capacity();

        // auto [segment_row_count, segment_status] = block_meta.segment_meta().GetRowCnt();
        // if (!segment_status.ok()) {
        //     return segment_status;
        // }

        if (actual_append) {
            // Status status = block_meta.segment_meta().SetRowCnt(segment_row_count + actual_append);
            // if (!status.ok()) {
            //     return status;
            // }
            segment_row_count += actual_append;
        }
        // segment_full = segment_row_count + actual_append >= block_meta.segment_meta().segment_capacity();
        segment_full = segment_row_count >= block_meta.segment_meta().segment_capacity();

        if (actual_append) {
            append_state->current_count_ += actual_append;
            append_state->current_block_offset_ += actual_append;
            if (append_state->current_block_offset_ == input_block->row_count()) {
                ++append_state->current_block_;
                append_state->current_block_offset_ = 0;
            }
        }
        break;
    }
    return Status::OK();
}

Status NewTxn::AppendInBlock(BlockMeta &block_meta, SizeT block_offset, SizeT append_rows, const DataBlock *input_block, SizeT input_offset) {
    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    // BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    // BufferObj *version_buffer = nullptr;
    // {
    //     String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + String(BlockVersion::PATH);
    //     version_buffer = buffer_mgr->GetBufferObject(version_filepath);
    //     if (version_buffer == nullptr) {
    //         return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
    //     }
    // }
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
        SizeT chunk_size = var_buffer_mgr->TotalSize();
        Status status = column_meta.SetChunkOffset(chunk_size);
        if (!status.ok()) {
            return status;
        }
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
    status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, range_state);
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

        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        for (ColumnID column_id : column_ids) {
            auto iter = std::find_if(column_defs_ptr->begin(), column_defs_ptr->end(), [&](const SharedPtr<ColumnDef> &column_def) {
                return ColumnID(column_def->id()) == column_id;
            });
            if (iter == column_defs_ptr->end()) {
                UnrecoverableError(fmt::format("Column {} not found in table meta", column_id));
            }
            SharedPtr<ColumnDef> column_def = *iter;
            new_catalog->AddCleanedMeta(commit_ts,
                                        MakeUnique<ColumnMetaKey>(block_meta.segment_meta().table_meta().db_id_str(),
                                                                  block_meta.segment_meta().table_meta().table_id_str(),
                                                                  block_meta.segment_meta().segment_id(),
                                                                  block_meta.block_id(),
                                                                  column_def));
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

Status NewTxn::CheckpointTableData(TableMeeta &table_meta, const CheckpointOption &option) {
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
                return status;
            }

            bool flush_version = false;
            bool flush_column = false;
            {
                std::shared_lock<std::shared_mutex> lock(block_lock->mtx_);
                if (block_lock->checkpoint_ts_ < std::min(option.checkpoint_ts_, block_lock->max_ts_)) {
                    flush_version = true;
                }
                if (block_lock->checkpoint_ts_ < std::min(option.checkpoint_ts_, block_lock->min_ts_)) {
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
        }
    }

    return Status::OK();
}

Status NewTxn::CommitImport(WalCmdImportV2 *import_cmd) {
    Status status;
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_id_str = import_cmd->db_id_;
    const String &table_id_str = import_cmd->table_id_;

    WalSegmentInfo &segment_info = import_cmd->segment_info_;

    {
        KVStore *kv_store = txn_mgr_->kv_store();
        UniquePtr<KVInstance> kv_instance = kv_store->GetInstance();
        TableMeeta table_meta(db_id_str, table_id_str, *kv_instance, begin_ts);
        auto [col_defs, col_def_status] = table_meta.GetColumnDefs();
        if (!col_def_status.ok()) {
            return col_def_status;
        }

        if (segment_info.column_count_ != col_defs->size()) {
            return Status::ColumnCountMismatch(
                fmt::format("Column count mismatch, expect: {}, actual: {}", col_defs->size(), segment_info.column_count_));
        }
    }

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_.get(), begin_ts);
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

Status NewTxn::CommitAppend(WalCmdAppendV2 *append_cmd, KVInstance *kv_instance) {
    Status status;
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const String &db_id_str = append_cmd->db_id_;
    const String &table_id_str = append_cmd->table_id_;

    NewTxnTableStore1 *txn_table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    AppendState *append_state = txn_table_store->append_state();
    if (append_state == nullptr) {
        status = txn_table_store->Append(append_cmd->block_);
        if (!status.ok()) {
            return status;
        }
        append_state = txn_table_store->append_state();
        append_state->Finalize();
    }

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance, begin_ts);
    Optional<SegmentMeta> segment_meta;
    SegmentID unsealed_segment_id = 0;
    status = table_meta.GetUnsealedSegmentID(unsealed_segment_id);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kNotFound) {
            return status;
        }
        status = NewCatalog::AddNewSegment1(table_meta, commit_ts, segment_meta);
        if (!status.ok()) {
            return status;
        }
        SegmentID segment_id = segment_meta->segment_id();
        status = table_meta.SetUnsealedSegmentID(segment_id);
        if (!status.ok()) {
            return status;
        }
    } else {
        segment_meta.emplace(unsealed_segment_id, table_meta);
    }
    if (segment_meta->segment_id() != append_cmd->row_ranges_.front().first.segment_id_) {
        UnrecoverableError(fmt::format("Segment id mismatch, expect: {}, actual: {}",
                                       append_cmd->row_ranges_.front().first.segment_id_,
                                       segment_meta->segment_id()));
    }

    Vector<BlockID> *block_ids_ptr = nullptr;
    std::tie(block_ids_ptr, status) = segment_meta->GetBlockIDs1();
    if (!status.ok()) {
        return status;
    }
    Optional<BlockMeta> block_meta;
    if (block_ids_ptr->empty()) {
        status = NewCatalog::AddNewBlock1(*segment_meta, commit_ts, block_meta);
        if (!status.ok()) {
            return status;
        }
    } else {
        BlockID block_id = block_ids_ptr->back();
        block_meta.emplace(block_id, segment_meta.value());
    }

    while (true) {
        bool block_full = false;
        bool segment_full = false;
        status = this->PrepareAppendInBlock(*block_meta, append_state, block_full, segment_full);
        if (!status.ok()) {
            return status;
        }
        if (append_state->Finished()) {
            break;
        }
        if (segment_full) {
            append_state->sealed_segments_.push_back(segment_meta->segment_id());
            status = NewCatalog::AddNewSegment1(table_meta, commit_ts, segment_meta);
            if (!status.ok()) {
                return status;
            }

            status = NewCatalog::AddNewBlock1(*segment_meta, commit_ts, block_meta);
            if (!status.ok()) {
                return status;
            }
        } else if (block_full) {
            status = NewCatalog::AddNewBlock1(*segment_meta, commit_ts, block_meta);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status NewTxn::PostCommitAppend(const WalCmdAppendV2 *append_cmd, KVInstance *kv_instance) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    Status status = Status::OK();
    const String &db_id_str = append_cmd->db_id_;
    const String &table_id_str = append_cmd->table_id_;

    NewTxnTableStore1 *txn_table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    const AppendState *append_state = txn_table_store->append_state();
    if (append_state == nullptr) {
        return Status::OK();
    }
    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance, begin_ts);
    Optional<SegmentMeta> segment_meta;
    Optional<BlockMeta> block_meta;
    for (const AppendRange &range : append_state->append_ranges_) {
        const DataBlock *data_block = append_state->blocks_[range.data_block_idx_].get();
        SegmentID segment_id = range.segment_id_;
        if (!segment_meta || segment_meta->segment_id() != segment_id) {
            segment_meta.emplace(segment_id, table_meta);
            block_meta.reset();
        }
        BlockID block_id = range.block_id_;
        if (!block_meta || block_meta->block_id() != block_id) {
            block_meta.emplace(block_id, segment_meta.value());
        }
        status = this->AppendInBlock(*block_meta, range.start_offset_, range.row_count_, data_block, range.data_block_offset_);
        if (!status.ok()) {
            return status;
        }
    }
    if (!this->IsReplay()) {
        Vector<String> *index_id_strs = nullptr;
        {
            status = table_meta.GetIndexIDs(index_id_strs, nullptr);
            if (!status.ok()) {
                return status;
            }
        }
        for (SizeT i = 0; i < index_id_strs->size(); ++i) {
            const String &index_id_str = (*index_id_strs)[i];
            TableIndexMeeta table_index_meta(index_id_str, table_meta);
            status = this->AppendIndex(table_index_meta, append_state->append_ranges_);
            if (!status.ok()) {
                return status;
            }
        }
    }

    for (SegmentID segment_id : append_state->sealed_segments_) {
        SegmentMeta segment_meta(segment_id, table_meta);
        BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(&segment_meta);
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitDelete(const WalCmdDeleteV2 *delete_cmd, KVInstance *kv_instance) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_id_str = delete_cmd->db_id_;
    const String &table_id_str = delete_cmd->table_id_;

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance, begin_ts);

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

Status NewTxn::RollbackDelete(const WalCmdDeleteV2 *delete_cmd, KVInstance *kv_instance) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    const String &db_id_str = delete_cmd->db_id_;
    const String &table_id_str = delete_cmd->table_id_;

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance, begin_ts);

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

Status NewTxn::CommitCompact(WalCmdCompactV2 *compact_cmd) {
    Status status;
    const String &db_id_str = compact_cmd->db_id_;
    const String &table_id_str = compact_cmd->table_id_;
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
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

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_.get(), begin_ts);
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

    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    const Vector<SegmentID> &deprecated_ids = compact_cmd->deprecated_segment_ids_;

    for (SegmentID segment_id : deprecated_ids) {
        new_catalog->AddCleanedMeta(commit_ts, MakeUnique<SegmentMetaKey>(db_id_str, table_id_str, segment_id));
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
                    new_catalog->AddCleanedMeta(commit_ts, MakeUnique<SegmentIndexMetaKey>(db_id_str, table_id_str, index_id_str, segment_id));
                }
            }
            status = table_index_meta.RemoveSegmentIndexIDs(*segment_ids_ptr);
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

        Status status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
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

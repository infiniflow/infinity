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

module infinity_core:new_txn_data.impl;

import :new_txn;
import :new_txn_manager;
import :kv_store;
import :default_values;
import :buffer_obj;
import :infinity_exception;
import :infinity_context;
import :data_file_worker;
import :var_file_worker;
import :version_file_worker;
import :block_version;
import :buffer_handle;
import :vector_buffer;
import :logger;
import :var_buffer;
import :wal_entry;
import :data_access_state;
import :column_meta;
import :block_meta;
import :segment_meta;
import :table_meta;
import :table_index_meta;
import :segment_index_meta;
import :new_catalog;
import :meta_key;
import :db_meta;
import :build_fast_rough_filter_task;
import :base_expression;
import :cast_expression;
import :value_expression;
import :expression_binder;
import :cast_function;
import :bound_cast_func;
import :expression_state;
import :expression_evaluator;
import :base_txn_store;
import :catalog_cache;
import :kv_code;
import :bg_task;
import :dump_index_process;
import :mem_index;
import :base_memindex;
import :emvb_index_in_mem;
import :txn_context;
import :persist_result_handler;
import :virtual_store;

import std;
import third_party;

import column_def;
import row_id;
import constant_expr;
import data_type;

namespace infinity {

struct NewTxnCompactState {
    NewTxnCompactState() = default;

    static Status Make(TableMeta &table_meta, TxnTimeStamp commit_ts, NewTxnCompactState &state, SegmentID segment_id) {
        Status status = NewCatalog::AddNewSegmentWithID(table_meta, commit_ts, state.new_segment_meta_, segment_id);
        if (!status.ok()) {
            return status;
        }
        std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs_ptr;
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        state.column_cnt_ = column_defs_ptr->size();

        return Status::OK();
    };

    size_t column_cnt() const { return column_cnt_; }

    Status NextBlock() {
        Status status;

        if (block_meta_) {
            block_row_cnts_.push_back(cur_block_row_cnt_);
            segment_row_cnt_ += cur_block_row_cnt_;
            block_meta_.reset();
        }

        status = NewCatalog::AddNewBlock1(*new_segment_meta_, commit_ts_, block_meta_);
        if (!status.ok()) {
            return status;
        }
        cur_block_row_cnt_ = 0;

        column_vectors_.clear();
        column_vectors_.resize(column_cnt_);

        for (size_t i = 0; i < column_cnt_; ++i) {
            ColumnMeta column_meta(i, *block_meta_);
            status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), 0, ColumnVectorMode::kReadWrite, column_vectors_[i]);
            if (!status.ok()) {
                return status;
            }
        }
        return status;
    }

    Status FinalizeBlock() {
        if (block_meta_) {
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

                auto [data_size, status2] = column_meta.GetColumnSize(cur_block_row_cnt_, column_meta.get_column_def());
                if (!status2.ok()) {
                    return status;
                }
                buffer_obj->SetDataSize(data_size);

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
        return status;
    }

    TxnTimeStamp commit_ts_{};

    std::optional<SegmentMeta> new_segment_meta_{};
    std::optional<BlockMeta> block_meta_{};

    std::vector<size_t> block_row_cnts_;
    size_t segment_row_cnt_{};
    BlockOffset cur_block_row_cnt_{};
    std::vector<ColumnVector> column_vectors_;
    size_t column_cnt_{};
};

Status NewTxn::Import(const std::string &db_name, const std::string &table_name, const std::vector<std::shared_ptr<DataBlock>> &input_blocks) {
    Status status;
    std::vector<size_t> block_row_cnts{};

    for (size_t i = 0; i < input_blocks.size(); ++i) {
        std::vector<std::shared_ptr<DataType>> column_types;
        block_row_cnts.emplace_back(input_blocks[i]->row_count());
        status = WriteDataBlockToFile(db_name, table_name, input_blocks[i], i);
        if (!status.ok()) {
            return status;
        }
    }

    status = Import(db_name, table_name, block_row_cnts);
    return status;
}

Status NewTxn::Import(const std::string &db_name, const std::string &table_name, const std::vector<size_t> &block_row_cnts) {
    this->CheckTxn(db_name);

    Status status;
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    std::string import_tmp_dir = "import" + std::to_string(TxnID());
    std::string import_tmp_path = InfinityContext::instance().config()->TempDir() + "/" + import_tmp_dir;

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta_opt;
    std::string table_key;
    TxnTimeStamp create_timestamp;
    status = GetTableMeta(db_name, table_name, db_meta, table_meta_opt, create_timestamp, &table_key);
    if (!status.ok()) {
        return status;
    }
    TableMeta &table_meta = *table_meta_opt;
    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    u64 db_id = std::stoull(table_meta.db_id_str());
    u64 table_id = std::stoull(table_meta.table_id_str());
    SystemCache *system_cache = txn_mgr_->GetSystemCachePtr();
    std::vector<SegmentID> segment_ids;

    size_t block_cnt = block_row_cnts.size();
    size_t segment_count =
        block_cnt % DEFAULT_BLOCK_PER_SEGMENT == 0 ? block_cnt / DEFAULT_BLOCK_PER_SEGMENT : block_cnt / DEFAULT_BLOCK_PER_SEGMENT + 1;

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

    std::vector<std::unique_ptr<SegmentMeta>> segment_metas;
    segment_metas.reserve(segment_count);
    for (size_t segment_idx = 0; segment_idx < segment_count; ++segment_idx) {
        std::optional<SegmentMeta> segment_meta;
        status = NewCatalog::AddNewSegmentWithID(table_meta, fake_commit_ts, segment_meta, segment_ids[segment_idx]);
        if (!status.ok()) {
            return status;
        }
        segment_metas.emplace_back(std::make_unique<SegmentMeta>(segment_ids[segment_idx], table_meta));
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }

    base_txn_store_ = std::make_shared<ImportTxnStore>();
    ImportTxnStore *import_txn_store = static_cast<ImportTxnStore *>(base_txn_store_.get());
    import_txn_store->db_name_ = db_name;
    import_txn_store->db_id_str_ = table_meta.db_id_str();
    import_txn_store->table_name_ = table_name;
    import_txn_store->table_id_str_ = table_meta.table_id_str();
    import_txn_store->table_key_ = table_key;
    import_txn_store->table_id_ = std::stoull(table_meta.table_id_str());
    import_txn_store->import_tmp_path_ = import_tmp_path;

    std::vector<size_t> segment_row_cnts(segment_count, 0);
    std::vector<std::vector<size_t>> block_row_cnts_in_seg(segment_count);
    for (size_t input_block_idx = 0; input_block_idx < block_cnt; ++input_block_idx) {
        std::optional<BlockMeta> block_meta;
        size_t segment_idx = input_block_idx / DEFAULT_BLOCK_PER_SEGMENT;
        size_t block_idx = input_block_idx % DEFAULT_BLOCK_PER_SEGMENT;
        status = NewCatalog::AddNewBlock1(*segment_metas[segment_idx], fake_commit_ts, block_meta);
        if (!status.ok()) {
            return status;
        }

        // Rename the data block
        std::string old_block_dir = fmt::format("db_{}/tbl_{}/seg_{}/blk_{}", db_meta->db_id_str(), table_meta.table_id(), segment_idx, block_idx);
        std::string old_block_path = InfinityContext::instance().config()->TempDir() + "/" + import_tmp_dir + "/" + old_block_dir;
        std::string new_block_dir = *block_meta->GetBlockDir();
        std::string new_block_path = InfinityContext::instance().config()->DataDir() + "/" + new_block_dir;

        std::vector<std::string> import_file_paths{};
        for (const auto &entry : std::filesystem::directory_iterator(old_block_path)) {
            std::string file_name = entry.path().filename().string();
            import_file_paths.emplace_back(new_block_path + "/" + file_name);
        }

        PersistenceManager *pm = InfinityContext::instance().persistence_manager();
        if (pm != nullptr) {
            PersistResultHandler handler(pm);
            for (const auto &entry : std::filesystem::directory_iterator(old_block_path)) {
                std::string file_name = entry.path().filename().string();
                std::string src_path = old_block_path + "/" + file_name;
                std::string dest_path = new_block_path + "/" + file_name;

                PersistWriteResult persist_result = pm->Persist(dest_path, src_path);
                handler.HandleWriteResult(persist_result);
                import_txn_store->import_file_names_.emplace_back(new_block_dir + "/" + file_name);
            }
        } else {
            Status rename_status = VirtualStore::Rename(old_block_path, new_block_path);
            if (!rename_status.ok()) {
                return rename_status;
            }
        }

        // Change type and status of buffer object of the import data files
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        for (auto &import_file : import_file_paths) {
            if (import_file.ends_with(".col") || import_file.ends_with("_out")) {
                buffer_mgr->ChangeBufferObjectState(import_file);
            }
        }

        block_row_cnts_in_seg[segment_idx].push_back(block_row_cnts[input_block_idx]);
        segment_row_cnts[segment_idx] += block_row_cnts[input_block_idx];
        import_txn_store->row_count_ += block_row_cnts[input_block_idx];
    }

    if (VirtualStore::Exists(import_tmp_path)) {
        Status remove_status = VirtualStore::RemoveDirectory(import_tmp_path);
        if (!remove_status.ok()) {
            LOG_WARN(fmt::format("Failed to remove import temp directory: {}", import_tmp_path));
        }
    }

    std::vector<WalSegmentInfo> segment_infos;
    segment_infos.reserve(segment_count);
    for (size_t segment_idx = 0; segment_idx < segment_count; ++segment_idx) {
        WalSegmentInfo segment_info(*segment_metas[segment_idx], begin_ts);
        for (size_t i = 0; i < block_row_cnts_in_seg[segment_idx].size(); ++i) {
            segment_info.block_infos_[i].row_count_ = block_row_cnts_in_seg[segment_idx][i];
        }
        segment_info.row_count_ = segment_row_cnts[segment_idx];
        status = this->AddSegmentVersion(segment_info, *segment_metas[segment_idx]);
        if (!status.ok()) {
            return status;
        }
        segment_infos.emplace_back(segment_info);
    }

    import_txn_store->segment_infos_.insert(import_txn_store->segment_infos_.end(), segment_infos.begin(), segment_infos.end());
    import_txn_store->segment_ids_.insert(import_txn_store->segment_ids_.end(), segment_ids.begin(), segment_ids.end());

    // index
    std::vector<std::string> *index_id_strs_ptr = nullptr;
    std::vector<std::string> *index_names_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
    if (!status.ok()) {
        return status;
    }

    for (size_t i = 0; i < index_id_strs_ptr->size(); ++i) {
        const std::string &index_id_str = (*index_id_strs_ptr)[i];
        const std::string &index_name = (*index_names_ptr)[i];
        import_txn_store->index_names_.emplace_back(index_name);
        import_txn_store->index_ids_str_.emplace_back(index_id_str);
        import_txn_store->index_ids_.emplace_back(std::stoull(index_id_str));
    }

    for (size_t i = 0; i < index_id_strs_ptr->size(); ++i) {
        const std::string &index_id_str = (*index_id_strs_ptr)[i];
        const std::string &index_name = (*index_names_ptr)[i];
        TableIndexMeta table_index_meta(index_id_str, index_name, table_meta);

        for (size_t segment_idx = 0; segment_idx < segment_count; ++segment_idx) {
            size_t segment_row_cnt = segment_row_cnts[segment_idx];
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
    std::string segment_id_key = KeyEncode::CatalogTableSegmentKey(import_cmd->db_id_, import_cmd->table_id_, import_cmd->segment_info_.segment_id_);
    std::string commit_ts_str;
    Status status = kv_instance_->Get(segment_id_key, commit_ts_str);
    if (status.ok()) {
        TxnTimeStamp commit_ts_from_kv = std::stoull(commit_ts_str);
        if (commit_ts == commit_ts_from_kv) {
            LOG_WARN(fmt::format("Skipping replay import: Segment {} already exists in table {} of database {} with commit ts {}, txn: {}.",
                                 import_cmd->segment_info_.segment_id_,
                                 import_cmd->table_name_,
                                 import_cmd->db_name_,
                                 commit_ts,
                                 txn_id));
            const WalSegmentInfo &segment_info = import_cmd->segment_info_;
            TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

            std::shared_ptr<DBMeta> db_meta;
            std::shared_ptr<TableMeta> table_meta_opt;
            TxnTimeStamp create_timestamp;
            status = GetTableMeta(import_cmd->db_name_, import_cmd->table_name_, db_meta, table_meta_opt, create_timestamp);
            if (!status.ok()) {
                return status;
            }
            TableMeta &table_meta = *table_meta_opt;
            status = NewCatalog::LoadImportedOrCompactedSegment(table_meta, segment_info, fake_commit_ts);
            if (!status.ok()) {
                return status;
            }
            return Status::OK();
        } else {
            LOG_ERROR(fmt::format("Replay import: Segment {} already exists in table {} of database {} with commit ts {}, but replaying with commit "
                                  "ts {}, txn: {}.",
                                  import_cmd->segment_info_.segment_id_,
                                  import_cmd->table_name_,
                                  import_cmd->db_name_,
                                  commit_ts_from_kv,
                                  commit_ts,
                                  txn_id));
            return Status::UnexpectedError("Segment already exists with different commit timestamp");
        }
    }

    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta_opt;
    TxnTimeStamp create_timestamp;
    status = GetTableMeta(import_cmd->db_name_, import_cmd->table_name_, db_meta, table_meta_opt, create_timestamp);
    if (!status.ok()) {
        return status;
    }
    TableMeta &table_meta = *table_meta_opt;

    const WalSegmentInfo &segment_info = import_cmd->segment_info_;

    status = NewCatalog::LoadFlushedSegment2(table_meta, segment_info, fake_commit_ts);
    if (!status.ok()) {
        return status;
    }

    return PrepareCommitReplayImport(import_cmd);
}

Status NewTxn::Append(const std::string &db_name, const std::string &table_name, const std::shared_ptr<DataBlock> &input_block) {
    this->CheckTxn(db_name);

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta;
    std::string table_key;
    TxnTimeStamp create_timestamp;
    auto status = GetTableMeta(db_name, table_name, db_meta, table_meta, create_timestamp, &table_key);
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ != nullptr) {
        return Status::UnexpectedError("txn store is not null");
    }
    base_txn_store_ = std::make_shared<AppendTxnStore>();
    auto *append_txn_store = static_cast<AppendTxnStore *>(base_txn_store_.get());
    append_txn_store->db_name_ = db_name;
    append_txn_store->db_id_str_ = table_meta->db_id_str();
    append_txn_store->db_id_ = std::stoull(table_meta->db_id_str());
    append_txn_store->table_name_ = table_name;
    append_txn_store->table_id_str_ = table_meta->table_id_str();
    append_txn_store->table_id_ = std::stoull(table_meta->table_id_str());
    append_txn_store->input_block_ = input_block;
    // append_txn_store->row_ranges_ will be populated after conflict check

    std::string operation_msg =
        fmt::format("APPEND table {}.{} (db_id: {}, table_id: {})", db_name, table_name, db_meta->db_id_str(), table_meta->table_id_str());
    txn_context_ptr_->AddOperation(std::make_shared<std::string>(operation_msg));

    return AppendInner(db_name, table_name, table_key, *table_meta, input_block);
}

Status NewTxn::Append(const TableInfo &table_info, const std::shared_ptr<DataBlock> &input_block) {
    return Append(*table_info.db_name_, *table_info.table_name_, input_block);
}

Status NewTxn::AppendInner(const std::string &db_name,
                           const std::string &table_name,
                           const std::string &table_key,
                           TableMeta &table_meta,
                           const std::shared_ptr<DataBlock> &input_block) {

    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs{nullptr};
    {
        auto [col_defs, col_def_status] = table_meta.GetColumnDefs();
        if (!col_def_status.ok()) {
            return col_def_status;
        }
        column_defs = col_defs;
    }
    size_t column_count = column_defs->size();

    if (input_block->column_count() != column_count) {
        std::string err_msg = fmt::format("Attempt to insert different column count data block into transaction table store");
        LOG_ERROR(err_msg);

        return Status::ColumnCountMismatch(err_msg);
    }

    std::vector<std::shared_ptr<DataType>> column_types;
    for (size_t col_id = 0; col_id < column_count; ++col_id) {
        column_types.emplace_back((*column_defs)[col_id]->type());
        if (*column_types.back() != *input_block->column_vectors[col_id]->data_type()) {
            LOG_ERROR(fmt::format("Attempt to insert different type data into transaction table store"));
            return Status::DataTypeMismatch(column_types.back()->ToString(), input_block->column_vectors[col_id]->data_type()->ToString());
        }
    }

    return Status::OK();
}

Status NewTxn::Delete(const std::string &db_name, const std::string &table_name, const std::vector<RowID> &row_ids) {
    this->CheckTxn(db_name);

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta_opt;
    std::string table_key;
    TxnTimeStamp create_timestamp;
    auto status = GetTableMeta(db_name, table_name, db_meta, table_meta_opt, create_timestamp, &table_key);
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ == nullptr) {
        base_txn_store_ = std::make_shared<DeleteTxnStore>();
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
    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta;
    TxnTimeStamp create_timestamp;
    Status status = GetTableMeta(delete_cmd->db_name_, delete_cmd->table_name_, db_meta, table_meta, create_timestamp);
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

Status NewTxn::DeleteInner(const std::string &db_name, const std::string &table_name, TableMeta &table_meta, const std::vector<RowID> &row_ids) {
    auto delete_command = std::make_shared<WalCmdDeleteV2>(db_name, table_meta.db_id_str(), table_name, table_meta.table_id_str(), row_ids);
    auto wal_command = static_pointer_cast<WalCmd>(delete_command);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(std::make_shared<std::string>(delete_command->ToString()));

    return Status::OK();
}

Status NewTxn::Update(const std::string &db_name,
                      const std::string &table_name,
                      const std::shared_ptr<DataBlock> &input_block,
                      const std::vector<RowID> &row_ids) {
    this->CheckTxn(db_name);

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta;
    std::string table_key;
    TxnTimeStamp create_timestamp;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta, create_timestamp, &table_key);
    if (!status.ok()) {
        return status;
    }

    // Put the data into local txn store
    if (base_txn_store_ == nullptr) {
        base_txn_store_ = std::make_shared<UpdateTxnStore>();
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

    std::string operation_msg =
        fmt::format("UPDATE table {}.{} (db_id: {}, table_id: {})", db_name, table_name, db_meta->db_id_str(), table_meta->table_id_str());
    txn_context_ptr_->AddOperation(std::make_shared<std::string>(operation_msg));

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

Status NewTxn::Compact(const std::string &db_name, const std::string &table_name, const std::vector<SegmentID> &segment_ids) {

    //    LOG_INFO(fmt::format("Start to compact segment ids: {}", segment_ids.size()));
    LOG_INFO(fmt::format("Compact db_name: {}, table_name: {}, segment ids: {}", db_name, table_name, fmt::join(segment_ids, " ")));

    this->CheckTxn(db_name);
    if (segment_ids.empty()) {
        return Status::UnexpectedError("No segment is given in compact operation");
    }

    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta_opt;
    std::string table_key;
    TxnTimeStamp create_timestamp;
    Status status = GetTableMeta(db_name, table_name, db_meta, table_meta_opt, create_timestamp, &table_key);
    if (!status.ok()) {
        return status;
    }

    TableMeta &table_meta = *table_meta_opt;
    status = table_meta.CheckSegments(segment_ids);
    if (!status.ok()) {
        return status;
    }

    // Fake commit timestamp, in prepare commit phase, it will be replaced by real commit timestamp
    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    SystemCache *system_cache = txn_mgr_->GetSystemCachePtr();
    u64 db_id = std::stoull(table_meta.db_id_str());
    u64 table_id = std::stoull(table_meta.table_id_str());
    std::vector<SegmentID> new_segment_ids;
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

    LOG_TRACE(fmt::format("To compact segments size: {}", segment_ids.size()));
    for (SegmentID segment_id : segment_ids) {
        SegmentMeta segment_meta(segment_id, table_meta);

        std::vector<BlockID> *block_ids_ptr;
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

    std::vector<std::string> *index_id_strs_ptr = nullptr;
    std::vector<std::string> *index_name_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_name_ptr);
    if (!status.ok()) {
        return status;
    }

    {
        // Put the data into local txn store
        if (base_txn_store_ != nullptr) {
            return Status::UnexpectedError("txn store is not null");
        }
        std::vector<WalSegmentInfo> segment_infos;
        segment_infos.emplace_back(*compact_state.new_segment_meta_, begin_ts);

        base_txn_store_ = std::make_shared<CompactTxnStore>();
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

        std::vector<SegmentID> deprecated_segment_ids = segment_ids;
        {
            WalSegmentInfo &segment_info = segment_infos.back();
            segment_info.row_count_ = compact_state.segment_row_cnt_;
            if (segment_info.block_infos_.size() != compact_state.block_row_cnts_.size()) {
                UnrecoverableError(fmt::format("Block row count mismatch, expect: {}, actual: {}",
                                               segment_info.block_infos_.size(),
                                               compact_state.block_row_cnts_.size()));
            }
            for (size_t i = 0; i < segment_info.block_infos_.size(); ++i) {
                WalBlockInfo &block_info = segment_info.block_infos_[i];
                block_info.row_count_ = compact_state.block_row_cnts_[i];
            }
        }
        auto compact_command = std::make_shared<WalCmdCompactV2>(db_name,
                                                                 db_meta->db_id_str(),
                                                                 table_name,
                                                                 table_meta.table_id_str(),
                                                                 *index_name_ptr,
                                                                 *index_id_strs_ptr,
                                                                 std::move(segment_infos),
                                                                 std::move(deprecated_segment_ids));
        wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(compact_command));
        txn_context_ptr_->AddOperation(std::make_shared<std::string>(compact_command->ToString()));

        status = this->AddSegmentVersion(compact_command->new_segment_infos_[0], *compact_state.new_segment_meta_);
        if (!status.ok()) {
            return status;
        }
    }

    CompactTxnStore *compact_txn_store = static_cast<CompactTxnStore *>(base_txn_store_.get());
    for (size_t i = 0; i < index_id_strs_ptr->size(); ++i) {
        const std::string &index_id_str = (*index_id_strs_ptr)[i];
        const std::string &index_name = (*index_name_ptr)[i];
        compact_txn_store->index_names_.emplace_back(index_name);
        compact_txn_store->index_ids_str_.emplace_back(index_id_str);
        compact_txn_store->index_ids_.emplace_back(std::stoull(index_id_str));
    }

    //    LOG_TRACE(fmt::format("To populate index: {}", index_id_strs_ptr->size()));
    for (size_t i = 0; i < index_id_strs_ptr->size(); ++i) {
        const std::string &index_id_str = (*index_id_strs_ptr)[i];
        const std::string &index_name = (*index_name_ptr)[i];
        TableIndexMeta table_index_meta(index_id_str, index_name, table_meta);

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

Status NewTxn::CheckTableIfDelete(TableMeta &table_meta, bool &has_delete) {
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    Status status = NewCatalog::CheckTableIfDelete(table_meta, begin_ts, has_delete);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewTxn::ReplayCompact(WalCmdCompactV2 *compact_cmd, TxnTimeStamp commit_ts, i64 txn_id) {

    std::optional<bool> skip_cmd = std::nullopt;
    for (const WalSegmentInfo &segment_info : compact_cmd->new_segment_infos_) {
        std::string segment_id_key = KeyEncode::CatalogTableSegmentKey(compact_cmd->db_id_, compact_cmd->table_id_, segment_info.segment_id_);
        std::string commit_ts_str;
        Status status = kv_instance_->Get(segment_id_key, commit_ts_str);
        if (status.ok()) {
            TxnTimeStamp commit_ts_from_kv = std::stoull(commit_ts_str);
            if (commit_ts == commit_ts_from_kv) {
                if (skip_cmd.has_value() && !skip_cmd.value()) {
                    return Status::UnexpectedError("Compact segments replay are mismatched in timestamp");
                }
                LOG_WARN(fmt::format("Skipping replay compact: Segment {} already exists in table {} of database {} with commit ts {}, txn: {}.",
                                     segment_info.segment_id_,
                                     compact_cmd->table_name_,
                                     compact_cmd->db_name_,
                                     commit_ts,
                                     txn_id));

                for (const WalSegmentInfo &segment_info : compact_cmd->new_segment_infos_) {
                    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

                    std::shared_ptr<DBMeta> db_meta;
                    std::shared_ptr<TableMeta> table_meta_opt;
                    TxnTimeStamp create_timestamp;
                    status = GetTableMeta(compact_cmd->db_name_, compact_cmd->table_name_, db_meta, table_meta_opt, create_timestamp);
                    if (!status.ok()) {
                        return status;
                    }
                    TableMeta &table_meta = *table_meta_opt;
                    status = NewCatalog::LoadImportedOrCompactedSegment(table_meta, segment_info, fake_commit_ts);
                    if (!status.ok()) {
                        return status;
                    }
                }
                skip_cmd = true;
            } else {
                LOG_ERROR(
                    fmt::format("Replay compact: Segment {} already exists in table {} of database {} with commit ts {}, but replaying with commit "
                                "ts {}, txn: {}.",
                                segment_info.segment_id_,
                                compact_cmd->table_name_,
                                compact_cmd->db_name_,
                                commit_ts_from_kv,
                                commit_ts,
                                txn_id));
                return Status::UnexpectedError("Segment already exists with different commit timestamp");
            }
        }
    }
    // TODO: check if the removed segments and segment indexes are created.
    if (skip_cmd) {
        return Status::OK();
    }

    TxnTimeStamp fake_commit_ts = txn_context_ptr_->begin_ts_;

    std::shared_ptr<DBMeta> db_meta;
    std::shared_ptr<TableMeta> table_meta_opt;
    TxnTimeStamp create_timestamp;
    Status status = GetTableMeta(compact_cmd->db_name_, compact_cmd->table_name_, db_meta, table_meta_opt, create_timestamp);
    if (!status.ok()) {
        return status;
    }
    TableMeta &table_meta = *table_meta_opt;

    for (const WalSegmentInfo &segment_info : compact_cmd->new_segment_infos_) {
        status = NewCatalog::LoadFlushedSegment2(table_meta, segment_info, fake_commit_ts);
        if (!status.ok()) {
            return status;
        }
    }

    {
        std::vector<SegmentID> *segment_ids_ptr = nullptr;
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

Status NewTxn::AppendInBlock(BlockMeta &block_meta, size_t block_offset, size_t append_rows, const DataBlock *input_block, size_t input_offset) {
    std::shared_ptr<std::string> block_dir_ptr = block_meta.GetBlockDir();
    auto [version_buffer, status] = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }

    std::shared_ptr<BlockLock> block_lock;
    {
        status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
    }
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    {
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        block_lock->min_ts_ = std::min(block_lock->min_ts_, commit_ts);
        block_lock->max_ts_ = std::max(block_lock->max_ts_, commit_ts);

        // append in column file
        for (size_t column_idx = 0; column_idx < input_block->column_count(); ++column_idx) {
            const ColumnVector &column_vector = *input_block->column_vectors[column_idx];
            ColumnMeta column_meta(column_idx, block_meta);
            status = this->AppendInColumn(column_meta, block_offset, append_rows, column_vector, input_offset);
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
Status
NewTxn::AppendInColumn(ColumnMeta &column_meta, size_t dest_offset, size_t append_rows, const ColumnVector &column_vector, size_t source_offset) {
    ColumnVector dest_vec;
    {
        Status status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), dest_offset, ColumnVectorMode::kReadWrite, dest_vec);
        if (!status.ok()) {
            return status;
        }
    }
    dest_vec.AppendWith(column_vector, source_offset, append_rows);

    BufferObj *buffer_obj = nullptr;
    BufferObj *outline_buffer_obj = nullptr;
    Status status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
    if (!status.ok()) {
        return status;
    }

    auto [data_size, status2] = column_meta.GetColumnSize(dest_vec.Size(), column_meta.get_column_def());
    if (!status2.ok()) {
        return status;
    }
    buffer_obj->SetDataSize(data_size);

    if (VarBufferManager *var_buffer_mgr = dest_vec.buffer_->var_buffer_mgr(); var_buffer_mgr != nullptr) {
        //     Ensure buffer obj is loaded.
        size_t _ = var_buffer_mgr->TotalSize();
    }
    return Status::OK();
}

Status NewTxn::DeleteInBlock(BlockMeta &block_meta, const std::vector<BlockOffset> &block_offsets, std::vector<BlockOffset> &undo_block_offsets) {
    std::shared_ptr<std::string> block_dir_ptr = block_meta.GetBlockDir();
    Status status;
    BufferObj *version_buffer = nullptr;
    std::tie(version_buffer, status) = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }

    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    {
        std::shared_ptr<BlockLock> block_lock;
        status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        // delete in version file
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());
        undo_block_offsets.reserve(block_offsets.size());
        for (BlockOffset block_offset : block_offsets) {
            status = block_version->Delete(block_offset, commit_ts);
            if (!status.ok()) {
                return status;
            }
            undo_block_offsets.push_back(block_offset);
        }
        block_lock->max_ts_ = std::max(block_lock->max_ts_, commit_ts); // FIXME: remove max_ts, undo delete should not revert max_ts
    }
    return Status::OK();
}

Status NewTxn::RollbackDeleteInBlock(BlockMeta &block_meta, const std::vector<BlockOffset> &block_offsets) {
    std::shared_ptr<std::string> block_dir_ptr = block_meta.GetBlockDir();
    BufferObj *version_buffer = nullptr;
    {
        std::string version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + std::string(BlockVersion::PATH);
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        version_buffer = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer == nullptr) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
        }
    }

    {
        std::shared_ptr<BlockLock> block_lock;
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

Status NewTxn::PrintVersionInBlock(BlockMeta &block_meta, const std::vector<BlockOffset> &block_offsets, bool ignore_invisible) {
    std::shared_ptr<std::string> block_dir_ptr = block_meta.GetBlockDir();
    Status status;
    BufferObj *version_buffer = nullptr;
    std::tie(version_buffer, status) = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }

    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    {
        std::shared_ptr<BlockLock> block_lock;
        status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        // delete in version file
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());
        for (BlockOffset block_offset : block_offsets) {
            status = block_version->Print(begin_ts, block_offset, ignore_invisible);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status NewTxn::CompactBlock(BlockMeta &block_meta, NewTxnCompactState &compact_state) {
    NewTxnGetVisibleRangeState range_state;
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    auto status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts, commit_ts, range_state);
    if (!status.ok()) {
        return status;
    }

    size_t block_row_cnt = range_state.block_offset_end();

    size_t column_cnt = compact_state.column_cnt();
    std::vector<ColumnVector> column_vectors;
    column_vectors.resize(column_cnt);
    for (size_t column_id = 0; column_id < column_cnt; ++column_id) {
        ColumnMeta column_meta(column_id, block_meta);

        status = NewCatalog::GetColumnVector(column_meta,
                                             column_meta.get_column_def(),
                                             block_row_cnt,
                                             ColumnVectorMode::kReadOnly,
                                             column_vectors[column_id]);
        if (!status.ok()) {
            return status;
        }
    }

    std::pair<BlockOffset, BlockOffset> range;
    BlockOffset offset = 0;
    while (range_state.Next(offset, range)) {
        if (range.second == range.first) {
            offset = range.second;
            continue;
        }
        size_t append_size = 0;
        while (true) {
            if (!compact_state.block_meta_) {
                status = compact_state.NextBlock();
                if (!status.ok()) {
                    return status;
                }
            }
            append_size = std::min(static_cast<size_t>(range.second - range.first),
                                   compact_state.block_meta_->block_capacity() - compact_state.cur_block_row_cnt_);
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

        std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs_ptr;
        TableMeta &table_meta = block_meta.segment_meta().table_meta();
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }

        for (size_t column_id = 0; column_id < column_cnt; ++column_id) {
            compact_state.column_vectors_[column_id].AppendWith(column_vectors[column_id], range.first, append_size);
        }
        compact_state.cur_block_row_cnt_ += append_size;
        offset = range.first + append_size;
    }

    return Status::OK();
}

Status
NewTxn::AddColumnsData(TableMeta &table_meta, const std::vector<std::shared_ptr<ColumnDef>> &column_defs, const std::vector<u32> &column_idx_list) {
    Status status;
    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    if (segment_ids_ptr->empty()) {
        return Status::OK();
    }

    std::vector<Value> default_values;
    ExpressionBinder tmp_binder(nullptr);
    for (const auto &column_def : column_defs) {
        if (!column_def->default_value()) {
            return Status::NotSupport(fmt::format("Column {} has no default value", column_def->name()));
        }
        std::shared_ptr<ConstantExpr> default_expr = column_def->default_value();
        auto expr = tmp_binder.BuildValueExpr(*default_expr, nullptr, 0, false);
        auto *value_expr = static_cast<ValueExpression *>(expr.get());

        const std::shared_ptr<DataType> &column_type = column_def->type();
        if (value_expr->Type() == *column_type) {
            default_values.push_back(value_expr->GetValue());
        } else {
            BoundCastFunc cast = CastFunction::GetBoundFunc(value_expr->Type(), *column_type);
            std::shared_ptr<BaseExpression> cast_expr = std::make_shared<CastExpression>(cast, expr, *column_type);
            std::shared_ptr<ExpressionState> expr_state = ExpressionState::CreateState(cast_expr);
            std::shared_ptr<ColumnVector> output_column_vector = ColumnVector::Make(column_type);
            output_column_vector->Initialize(ColumnVectorType::kConstant, 1);
            ExpressionEvaluator evaluator;
            evaluator.Init(nullptr);
            evaluator.Execute(cast_expr, expr_state, output_column_vector);

            default_values.push_back(output_column_vector->GetValueByIndex(0));
        }
    }

    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        status = this->AddColumnsDataInSegment(segment_meta, column_defs, column_idx_list, default_values);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::AddColumnsDataInSegment(SegmentMeta &segment_meta,
                                       const std::vector<std::shared_ptr<ColumnDef>> &column_defs,
                                       const std::vector<u32> &column_idx_list,
                                       const std::vector<Value> &default_values) {

    auto [block_ids_ptr, status] = segment_meta.GetBlockIDs1();
    if (!status.ok()) {
        return status;
    }

    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta);
        status = this->AddColumnsDataInBlock(block_meta, column_defs, column_idx_list, default_values);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::AddColumnsDataInBlock(BlockMeta &block_meta,
                                     const std::vector<std::shared_ptr<ColumnDef>> &column_defs,
                                     const std::vector<u32> &column_idx_list,
                                     const std::vector<Value> &default_values) {
    auto [block_row_count, status] = block_meta.GetRowCnt1();
    if (!status.ok()) {
        return status;
    }

    LOG_TRACE("NewTxn::AddColumnsDataInBlock begin");
    size_t new_column_count = column_defs.size();
    for (size_t i = 0; i < new_column_count; ++i) {
        size_t column_idx = column_idx_list[i];
        const std::shared_ptr<ColumnDef> &column_def = column_defs[i];
        const Value &default_value = default_values[i];

        std::optional<ColumnMeta> column_meta;
        status = NewCatalog::AddNewBlockColumn(block_meta, column_idx, column_def, column_meta);
        if (!status.ok()) {
            return status;
        }

        ColumnVector column_vector;
        status = NewCatalog::GetColumnVector(*column_meta, column_def, 0 /*row_count*/, ColumnVectorMode::kReadWrite, column_vector);
        if (!status.ok()) {
            return status;
        }

        for (size_t j = 0; j < block_row_count; ++j) {
            column_vector.AppendValue(default_value);
        }

        BufferObj *buffer_obj = nullptr;
        BufferObj *outline_buffer_obj = nullptr;
        status = column_meta->GetColumnBuffer(buffer_obj, outline_buffer_obj);
        if (!status.ok()) {
            return status;
        }

        auto [data_size, status2] = column_meta->GetColumnSize(column_vector.Size(), column_def);
        if (!status2.ok()) {
            return status;
        }
        buffer_obj->SetDataSize(data_size);

        if (VarBufferManager *var_buffer_mgr = column_vector.buffer_->var_buffer_mgr(); var_buffer_mgr != nullptr) {
            //     Ensure buffer obj is loaded.
            size_t _ = var_buffer_mgr->TotalSize();
        }
        LOG_TRACE(
            fmt::format("NewTxn::AddColumnsDataInBlock: column name {}, column id {}, column_idx {}, default value {}, segment {}, block {}, rows {}",
                        column_defs[i]->name(),
                        column_defs[i]->id(),
                        column_idx,
                        default_value.ToString(),
                        block_meta.segment_meta().segment_id(),
                        block_meta.block_id(),
                        block_row_count));
    }

    std::shared_ptr<BlockLock> block_lock;
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

Status NewTxn::DropColumnsData(TableMeta &table_meta, const std::vector<ColumnID> &column_ids) {
    Status status;
    std::vector<SegmentID> *segment_ids_ptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    if (segment_ids_ptr->empty()) {
        return Status::OK();
    }

    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs_ptr;
    std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
    if (!status.ok()) {
        return status;
    }

    auto drop_columns_in_block = [&](BlockMeta &block_meta) {
        TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

        for (ColumnID column_id : column_ids) {
            auto iter = std::find_if(column_defs_ptr->begin(), column_defs_ptr->end(), [&](const std::shared_ptr<ColumnDef> &column_def) {
                return ColumnID(column_def->id()) == column_id;
            });
            if (iter == column_defs_ptr->end()) {
                UnrecoverableError(fmt::format("Column {} not found in table meta", column_id));
            }
            std::shared_ptr<ColumnDef> column_def = *iter;

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
        auto [block_ids, block_status] = segment_meta.GetBlockIDs1();
        if (!block_status.ok()) {
            return block_status;
        }
        for (BlockID block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);
            block_status = drop_columns_in_block(block_meta);
            if (!block_status.ok()) {
                return block_status;
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

Status NewTxn::CheckpointTable(TableMeta &table_meta, const CheckpointOption &option, CheckpointTxnStore *ckp_txn_store) {
    Status status;

    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    std::vector<BlockID> *block_ids_ptr = nullptr;
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return status;
        }
        for (BlockID block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            std::shared_ptr<BlockLock> block_lock;
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
            LOG_TRACE(fmt::format("NewTxn::CheckpointTable segment_id {}, block_id {}, flush_column {}, flush_version {}, option.checkpoint_ts_ {}, "
                                  "block min_ts {}, block "
                                  "max_ts {}, block checkpoint_ts {}",
                                  segment_id,
                                  block_id,
                                  flush_column,
                                  flush_version,
                                  option.checkpoint_ts_,
                                  block_lock->min_ts_,
                                  block_lock->max_ts_,
                                  block_lock->checkpoint_ts_));
            if (!flush_column or !flush_version) {
                continue;
            } else {
                auto flush_data_entry = std::make_shared<FlushDataEntry>(table_meta.db_id_str(), table_meta.table_id_str(), segment_id, block_id);
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

Status NewTxn::CheckpointTable(TableMeta &table_meta, const SnapshotOption &option, CheckpointTxnStore *ckp_txn_store) {
    Status status;
    std::shared_ptr<TableSnapshotInfo> table_snapshot_info;
    auto [db_name, table_name] = table_meta.GetDBTableName();
    std::tie(table_snapshot_info, status) = table_meta.MapMetaToSnapShotInfo(db_name, table_name);
    if (!status.ok()) {
        return status;
    }

    CreateTableSnapshotTxnStore *create_txn_store = static_cast<CreateTableSnapshotTxnStore *>(base_txn_store_.get());
    table_snapshot_info->snapshot_name_ = create_txn_store->snapshot_name_;

    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    std::string data_dir = InfinityContext::instance().config()->DataDir();
    std::string snapshot_dir = InfinityContext::instance().config()->SnapshotDir();
    std::string snapshot_name = table_snapshot_info->snapshot_name_;

    [[maybe_unused]] auto CreateSnapshotByIO = [&](const std::string &file) -> Status {
        if (pm != nullptr) {
            PersistResultHandler handler(pm);
            PersistReadResult result = pm->GetObjCache(file);
            DeferFn defer_fn([&]() {
                auto res = pm->PutObjCache(file);
                handler.HandleWriteResult(res);
            });

            const ObjAddr &obj_addr = handler.HandleReadResult(result);
            if (!obj_addr.Valid()) {
                UnrecoverableError(fmt::format("GetObjCache failed: {}", file));
            }

            std::string read_path = pm->GetObjPath(obj_addr.obj_key_);
            std::string write_path = fmt::format("{}/{}/{}", snapshot_dir, snapshot_name, file);
            LOG_TRACE(fmt::format("CreateSnapshotByIO, Read path: {}, Write path: {}", read_path, write_path));

            std::string write_dir = VirtualStore::GetParentPath(write_path);
            if (!VirtualStore::Exists(write_dir)) {
                VirtualStore::MakeDirectory(write_dir);
            }

            auto [read_handle, read_open_status] = VirtualStore::Open(read_path, FileAccessMode::kRead);
            if (!read_open_status.ok()) {
                UnrecoverableError(read_open_status.message());
            }

            auto seek_status = read_handle->Seek(obj_addr.part_offset_);
            if (!seek_status.ok()) {
                UnrecoverableError(seek_status.message());
            }

            auto file_size = obj_addr.part_size_;
            auto buffer = std::make_unique<char[]>(file_size);
            auto [nread, read_status] = read_handle->Read(buffer.get(), file_size);

            auto [write_handle, write_open_status] = VirtualStore::Open(write_path, FileAccessMode::kWrite);
            if (!write_open_status.ok()) {
                UnrecoverableError(write_open_status.message());
            }

            Status write_status = write_handle->Append(buffer.get(), file_size);
            if (!write_status.ok()) {
                UnrecoverableError(write_status.message());
            }
            write_handle->Sync();
        } else {
            std::string read_path = fmt::format("{}/{}", data_dir, file);
            std::string write_path = fmt::format("{}/{}/{}", snapshot_dir, snapshot_name, file);
            LOG_TRACE(fmt::format("CreateSnapshotByIO, Read path: {}, Write path: {}", read_path, write_path));

            Status status = VirtualStore::Copy(write_path, read_path);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
        }

        return Status::OK();
    };

    // auto CreateSnapshotByMapping = [&]() -> Status {
    //
    //
    // };

    auto data_start_time = std::chrono::high_resolution_clock::now();

    std::vector<BlockID> *block_ids_ptr = nullptr;
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return status;
        }
        for (BlockID block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);

            std::shared_ptr<BlockLock> block_lock;
            status = block_meta.GetBlockLock(block_lock);
            if (!status.ok()) {
                LOG_TRACE(fmt::format("NewTxn::CheckpointTable segment_id {}, block_id {}, got no BlockLock", segment_id, block_id));
                continue;
            }

            bool use_memory = false;
            {
                std::shared_lock<std::shared_mutex> lock(block_lock->mtx_);
                if (block_lock->checkpoint_ts_ < std::min(option.checkpoint_ts_, block_lock->max_ts_)) {
                    use_memory = true;
                }
            }
            LOG_TRACE(fmt::format("block: {}, use_memory: {}", block_id, use_memory ? "true" : "false"));

            {
                std::shared_ptr<std::string> block_dir_ptr = block_meta.GetBlockDir();
                BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

                std::string version_filepath =
                    InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + std::string(BlockVersion::PATH);
                BufferObj *version_buffer = buffer_mgr->GetBufferObject(version_filepath);
                if (version_buffer == nullptr) {
                    return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
                }

                VersionFileWorkerSaveCtx ctx(option.checkpoint_ts_);
                version_buffer->SaveSnapshot(table_snapshot_info, use_memory, ctx);
            }

            {
                std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs;
                std::tie(column_defs, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
                if (!status.ok()) {
                    return status;
                }

                NewTxnGetVisibleRangeState state;
                status = NewCatalog::GetBlockVisibleRange(block_meta, txn_context_ptr_->begin_ts_, txn_context_ptr_->commit_ts_, state);
                if (!status.ok()) {
                    return status;
                }

                std::pair<BlockOffset, BlockOffset> range;
                BlockOffset row_cnt = 0;
                while (true) {
                    bool has_next = state.Next(row_cnt, range);
                    if (!has_next) {
                        break;
                    }
                    row_cnt = range.second;
                    LOG_TRACE(fmt::format("range.first: {}, range.second: {}", range.first, range.second));
                }
                LOG_TRACE(fmt::format("row_cnt: {}", row_cnt));

                for (size_t column_idx = 0; column_idx < column_defs->size(); ++column_idx) {
                    ColumnMeta column_meta(column_idx, block_meta);
                    BufferObj *buffer_obj = nullptr;
                    BufferObj *outline_buffer_obj = nullptr;

                    status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
                    if (!status.ok()) {
                        return status;
                    }

                    size_t total_data_size = 0;
                    std::tie(total_data_size, status) = column_meta.GetColumnSize(row_cnt, (*column_defs)[column_idx]);

                    if (buffer_obj) {
                        buffer_obj->SaveSnapshot(table_snapshot_info, use_memory, {}, total_data_size);
                    }

                    if (outline_buffer_obj) {
                        outline_buffer_obj->SaveSnapshot(table_snapshot_info, use_memory);
                    }
                }
            }
        }
    }

    // End timing for data serialization
    auto data_end_time = std::chrono::high_resolution_clock::now();
    auto data_duration = std::chrono::duration_cast<std::chrono::milliseconds>(data_end_time - data_start_time);
    LOG_INFO(fmt::format("Saving data and version files took {} ms", data_duration.count()));

    {
        auto index_start_time = std::chrono::high_resolution_clock::now();

        std::vector<std::string> index_files = table_snapshot_info->GetIndexFiles();
        for (const auto &index_file : index_files) {
            status = CreateSnapshotByIO(index_file);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
        }

        auto index_end_time = std::chrono::high_resolution_clock::now();
        auto index_duration = std::chrono::duration_cast<std::chrono::milliseconds>(index_end_time - index_start_time);
        LOG_TRACE(fmt::format("Saving index files took {} ms", index_duration.count()));
    }

    {
        auto json_start_time = std::chrono::high_resolution_clock::now();

        nlohmann::json json_res = table_snapshot_info->CreateSnapshotMetadataJSON();
        std::string json_string = json_res.dump();

        std::string meta_path = fmt::format("{}/{}/{}.json", snapshot_dir, table_snapshot_info->snapshot_name_, table_snapshot_info->snapshot_name_);
        auto [snapshot_file_handle, meta_status] = VirtualStore::Open(meta_path, FileAccessMode::kWrite);
        if (!meta_status.ok()) {
            return meta_status;
        }

        status = snapshot_file_handle->Append(json_string.data(), json_string.size());
        if (!status.ok()) {
            return status;
        }
        snapshot_file_handle->Sync();

        auto json_end_time = std::chrono::high_resolution_clock::now();
        auto json_duration = std::chrono::duration_cast<std::chrono::milliseconds>(json_end_time - json_start_time);
        LOG_TRACE(fmt::format("Saving json files took {} ms", json_duration.count()));
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitImport(WalCmdImportV2 *import_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const std::string &db_id_str = import_cmd->db_id_;
    const std::string &table_id_str = import_cmd->table_id_;
    const std::string &table_name = import_cmd->table_name_;

    WalSegmentInfo &segment_info = import_cmd->segment_info_;
    TableMeta table_meta(db_id_str, table_id_str, table_name, this);
    SegmentMeta segment_meta(segment_info.segment_id_, table_meta);

    Status status = table_meta.CommitSegment(segment_info.segment_id_, commit_ts);
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

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    if (pm != nullptr) {
        // When all data and index is write to disk, try to finalize the
        PersistResultHandler handler(pm);
        PersistWriteResult result = pm->CurrentObjFinalize();
        handler.HandleWriteResult(result);
    }

    return Status::OK();
}

Status NewTxn::PrepareCommitReplayImport(WalCmdImportV2 *import_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    const std::string &db_id_str = import_cmd->db_id_;
    const std::string &table_id_str = import_cmd->table_id_;
    const std::string &table_name = import_cmd->table_name_;

    WalSegmentInfo &segment_info = import_cmd->segment_info_;
    TableMeta table_meta(db_id_str, table_id_str, table_name, this);
    SegmentMeta segment_meta(segment_info.segment_id_, table_meta);

    Status status = table_meta.CommitSegment(segment_info.segment_id_, commit_ts);
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

    BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(&segment_meta);

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    if (pm != nullptr) {
        // When all data and index is write to disk, try to finalize the
        PersistResultHandler handler(pm);
        PersistWriteResult result = pm->CurrentObjFinalize();
        handler.HandleWriteResult(result);
    }

    return Status::OK();
}

Status NewTxn::CommitBottomAppend(WalCmdAppendV2 *append_cmd) {
    Status status;
    const std::string &db_name = append_cmd->db_name_;
    const std::string &table_name = append_cmd->table_name_;
    const std::string &db_id_str = append_cmd->db_id_;
    const std::string &table_id_str = append_cmd->table_id_;
    TxnTimeStamp commit_ts = CommitTS();
    TableMeta table_meta(db_id_str, table_id_str, table_name, this);
    table_meta.SetDBTableName(db_name, table_name);
    std::optional<SegmentMeta> segment_meta;
    std::optional<BlockMeta> block_meta;
    size_t copied_row_cnt = 0;
    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs_ptr;
    std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    std::vector<std::shared_ptr<TableIndexMeta>> table_index_metas;
    std::vector<std::string> *index_name_strs = nullptr;
    if (!this->IsReplay()) {
        std::vector<std::string> *index_id_strs = nullptr;
        {
            status = table_meta.GetIndexIDs(index_id_strs, &index_name_strs);
            if (!status.ok()) {
                return status;
            }
        }
        for (size_t i = 0; i < index_id_strs->size(); ++i) {
            const std::string &index_id_str = (*index_id_strs)[i];
            const std::string &index_name_str = (*index_name_strs)[i];
            table_index_metas.push_back(std::make_shared<TableIndexMeta>(index_id_str, index_name_str, table_meta));
        }
    }

    // ensure append_cmd->row_ranges_ be block aligned
    std::vector<std::pair<RowID, u64>> append_ranges;
    auto calc_block_room = [](RowID row_id) -> u64 { return BLOCK_OFFSET_MASK - (row_id.segment_offset_ & BLOCK_OFFSET_MASK) + 1; };
    for (const std::pair<RowID, u64> &range : append_cmd->row_ranges_) {
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

    LOG_DEBUG("NewTxn::CommitBottomAppend begin");
    for (const std::pair<RowID, u64> &range : append_ranges) {
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
        size_t block_row_cnt;
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
            status = this->AppendIndex(*table_index_meta, range);
            if (!status.ok()) {
                return status;
            }
        }

        // If reach the end of segment, set it to sealed and build fast rough filter
        if (range.first.segment_offset_ + range.second == DEFAULT_SEGMENT_CAPACITY) {
            table_meta.DelUnsealedSegmentID();
            BuildFastRoughFilterTask::ExecuteOnNewSealedSegment(&segment_meta.value());

            for (size_t i = 0; i < table_index_metas.size(); ++i) {
                const std::string &index_name = (*index_name_strs)[i];
                std::shared_ptr<DumpMemIndexTask> dump_index_task = std::make_shared<DumpMemIndexTask>(db_name, table_name, index_name, segment_id);
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
    const std::string &db_id_str = delete_cmd->db_id_;
    const std::string &table_id_str = delete_cmd->table_id_;
    const std::string &table_name = delete_cmd->table_name_;

    TableMeta table_meta(db_id_str, table_id_str, table_name, this);

    std::optional<SegmentMeta> segment_meta;
    std::optional<BlockMeta> block_meta;

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
    const std::string &db_id_str = delete_txn_store->db_id_str_;
    const std::string &table_id_str = delete_txn_store->table_id_str_;
    const std::string &table_name = delete_txn_store->table_name_;

    TableMeta table_meta(db_id_str, table_id_str, table_name, this);

    std::optional<SegmentMeta> segment_meta;
    std::optional<BlockMeta> block_meta;

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
    const std::string &db_id_str = compact_cmd->db_id_;
    const std::string &table_id_str = compact_cmd->table_id_;
    const std::string &table_name = compact_cmd->table_name_;
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    std::vector<WalSegmentInfo> &segment_infos = compact_cmd->new_segment_infos_;
    if (segment_infos.empty()) {
        return Status::OK();
    }
    if (segment_infos.size() > 1) {
        UnrecoverableError("Not implemented");
    }
    WalSegmentInfo &segment_info = segment_infos[0];
    std::vector<SegmentID> new_segment_ids{segment_info.segment_id_};

    TableMeta table_meta(db_id_str, table_id_str, table_name, this);
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

    const std::vector<SegmentID> &deprecated_ids = compact_cmd->deprecated_segment_ids_;

    for (SegmentID segment_id : deprecated_ids) {
        auto ts_str = std::to_string(commit_ts);
        kv_instance_->Put(KeyEncode::DropSegmentKey(db_id_str, table_id_str, segment_id), ts_str);
    }
    {
        std::vector<std::string> *index_id_strs_ptr = nullptr;
        std::vector<std::string> *index_name_strs_ptr = nullptr;
        status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_name_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        size_t index_count = index_id_strs_ptr->size();
        for (size_t idx = 0; idx < index_count; ++idx) {
            const std::string &index_id_str = index_id_strs_ptr->at(idx);
            const std::string &index_name_str = index_name_strs_ptr->at(idx);

            TableIndexMeta table_index_meta(index_id_str, index_name_str, table_meta);
            std::vector<SegmentID> *segment_ids_ptr = nullptr;
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
        }
    }

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    if (pm != nullptr) {
        // When all data and index is write to disk, try to finalize the
        PersistResultHandler handler(pm);
        PersistWriteResult result = pm->CurrentObjFinalize();
        handler.HandleWriteResult(result);
    }

    return Status::OK();
}

Status NewTxn::CommitCheckpointTableData(TableMeta &table_meta, TxnTimeStamp checkpoint_ts) {
    Status status;

    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }

    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta);
        auto [block_ids, block_status] = segment_meta.GetBlockIDs1();
        if (!block_status.ok()) {
            return block_status;
        }
        for (BlockID block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta);

            std::shared_ptr<BlockLock> block_lock;
            status = block_meta.GetBlockLock(block_lock);
            if (!status.ok()) {
                return status;
            }

            {
                std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);
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
        std::shared_ptr<std::string> block_dir_ptr = block_meta.GetBlockDir();

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
    for (WalBlockInfo &block_info : segment_info.block_infos_) {
        BlockMeta block_meta(block_info.block_id_, segment_meta);
        std::shared_ptr<std::string> block_dir_ptr = block_meta.GetBlockDir();

        auto [version_buffer, status] = block_meta.GetVersionBuffer();
        if (!status.ok()) {
            return status;
        }
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());

        block_version->CommitAppend(save_ts, commit_ts);
        version_buffer->Save(VersionFileWorkerSaveCtx(commit_ts));

        std::shared_ptr<BlockLock> block_lock;
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
    }

    return Status::OK();
}

Status NewTxn::FlushVersionFile(BlockMeta &block_meta, TxnTimeStamp save_ts) {
    std::shared_ptr<std::string> block_dir_ptr = block_meta.GetBlockDir();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    BufferObj *version_buffer = nullptr;
    {
        std::string version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + std::string(BlockVersion::PATH);
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

    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    LOG_TRACE("NewTxn::FlushColumnFiles begin");
    for (size_t column_idx = 0; column_idx < column_defs->size(); ++column_idx) {
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
    LOG_TRACE("NewTxn::FlushColumnFiles end");
    return Status::OK();
}

Status NewTxn::TryToMmap(BlockMeta &block_meta, TxnTimeStamp save_ts, bool *to_mmap_ptr) {
    auto [row_cnt, status] = block_meta.GetRowCnt1();
    if (!status.ok()) {
        return status;
    }
    bool to_mmap = row_cnt >= block_meta.block_capacity();
    if (to_mmap_ptr) {
        *to_mmap_ptr = to_mmap;
    }
    if (to_mmap) {
        std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs;
        std::tie(column_defs, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        for (size_t column_idx = 0; column_idx < column_defs->size(); ++column_idx) {
            ColumnMeta column_meta(column_idx, block_meta);
            BufferObj *buffer_obj = nullptr;
            BufferObj *outline_buffer_obj = nullptr;

            status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
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

Status NewTxn::WriteDataBlockToFile(const std::string &db_name,
                                    const std::string &table_name,
                                    std::shared_ptr<DataBlock> input_block,
                                    const u64 &input_block_idx,
                                    std::vector<std::string> *object_paths) {
    Status status;
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    std::string import_tmp_dir = fmt::format("import{}", TxnID());
    std::string import_tmp_path_ = InfinityContext::instance().config()->TempDir() + "/" + import_tmp_dir;

    if (!input_block->Finalized()) {
        UnrecoverableError("Attempt to import unfinalized data block");
    }

    auto [table_info, status2] = GetTableInfo(db_name, table_name);
    if (!status2.ok()) {
        return status2;
    }

    size_t table_column_count = table_info->column_defs_.size();
    if (input_block->column_count() != table_column_count) {
        std::string err_msg = fmt::format("Attempt to import different column count data block into transaction table store");
        LOG_ERROR(err_msg);
        return Status::ColumnCountMismatch(err_msg);
    }

    std::vector<std::shared_ptr<DataType>> column_types;
    for (size_t col_id = 0; col_id < table_column_count; ++col_id) {
        column_types.emplace_back(table_info->column_defs_[col_id]->type());
        if (*column_types.back() != *input_block->column_vectors[col_id]->data_type()) {
            LOG_ERROR(fmt::format("Attempt to import different type data into transaction table store"));
            return Status::DataTypeMismatch(column_types.back()->ToString(), input_block->column_vectors[col_id]->data_type()->ToString());
        }
    }

    u32 row_cnt = input_block->row_count();
    LOG_INFO(fmt::format("Writing block {}, row_count: {}", input_block_idx, row_cnt));
    size_t segment_idx = input_block_idx / DEFAULT_BLOCK_PER_SEGMENT;
    size_t block_idx = input_block_idx % DEFAULT_BLOCK_PER_SEGMENT;

    for (size_t i = 0; i < input_block->column_count(); ++i) {
        std::shared_ptr<ColumnVector> col = input_block->column_vectors[i];
        auto col_def = table_info->column_defs_[i];

        BufferObj *buffer_obj = nullptr;
        BufferObj *outline_buffer_obj = nullptr;
        ColumnID column_id = col_def->id();
        std::shared_ptr<std::string> col_filename = std::make_shared<std::string>(fmt::format("{}.col", column_id));

        size_t total_data_size = 0;
        if (col_def->type()->type() == LogicalType::kBoolean) {
            total_data_size = (DEFAULT_BLOCK_CAPACITY + 7) / 8;
        } else {
            total_data_size = DEFAULT_BLOCK_CAPACITY * col_def->type()->Size();
        }

        std::shared_ptr<std::string> block_dir = std::make_shared<std::string>(
            fmt::format("db_{}/tbl_{}/seg_{}/blk_{}", table_info->db_id_, table_info->table_id_, segment_idx, block_idx));
        auto file_worker1 = std::make_unique<DataFileWorker>(std::make_shared<std::string>(import_tmp_path_),
                                                             std::make_shared<std::string>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir,
                                                             col_filename,
                                                             total_data_size,
                                                             buffer_mgr->persistence_manager());

        if (object_paths != nullptr) {
            std::string file_path1 = file_worker1->GetFilePath();
            object_paths->push_back(file_path1);
        }

        buffer_obj = buffer_mgr->AllocateBufferObject(std::move(file_worker1));

        VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
        if (buffer_type == VectorBufferType::kVarBuffer) {
            std::shared_ptr<std::string> outline_filename = std::make_shared<std::string>(fmt::format("col_{}_out", column_id));
            auto file_worker2 = std::make_unique<VarFileWorker>(std::make_shared<std::string>(import_tmp_path_),
                                                                std::make_shared<std::string>(InfinityContext::instance().config()->TempDir()),
                                                                block_dir,
                                                                outline_filename,
                                                                0,
                                                                buffer_mgr->persistence_manager());

            if (object_paths != nullptr) {
                std::string file_path2 = file_worker2->GetFilePath();
                object_paths->push_back(file_path2);
            }
            outline_buffer_obj = buffer_mgr->AllocateBufferObject(std::move(file_worker2));
        }

        col->SetToCatalog(buffer_obj, outline_buffer_obj, ColumnVectorMode::kReadWrite);

        size_t data_size = 0;
        if (col_def->type()->type() == LogicalType::kBoolean) {
            data_size = (row_cnt + 7) / 8;
        } else {
            data_size = row_cnt * col_def->type()->Size();
        }
        buffer_obj->SetDataSize(data_size);

        buffer_obj->Save();
        if (outline_buffer_obj) {
            outline_buffer_obj->Save();
        }
    }

    return Status::OK();
}

} // namespace infinity

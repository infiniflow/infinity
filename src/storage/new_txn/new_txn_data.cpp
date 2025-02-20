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
import kv_code;
import third_party;
import default_values;
import buffer_obj;
import infinity_exception;
import infinity_context;
import version_file_worker;
import data_file_worker;
import var_file_worker;
import block_version;
import buffer_handle;
import vector_buffer;
import logger;
import var_buffer;
import wal_entry;

namespace infinity {

void NewTxnGetVisibleRangeState::Init(SharedPtr<BlockLock> block_lock, BufferHandle version_buffer_handle, TxnTimeStamp begin_ts) {
    block_lock_ = std::move(block_lock);
    version_buffer_handle_ = std::move(version_buffer_handle);
    begin_ts_ = begin_ts;
    {
        std::shared_lock<std::shared_mutex> lock(block_lock_->mtx_);
        const auto *block_version = reinterpret_cast<const BlockVersion *>(version_buffer_handle_.GetData());
        block_offset_end_ = block_version->GetRowCount(begin_ts_);
    }
}

bool NewTxnGetVisibleRangeState::Next(BlockOffset block_offset_begin, Pair<BlockOffset, BlockOffset> &visible_range) {
    if (block_offset_begin == block_offset_end_) {
        return false;
    }
    [[maybe_unused]] const auto *block_version = reinterpret_cast<const BlockVersion *>(version_buffer_handle_.GetData());

    std::shared_lock<std::shared_mutex> lock(block_lock_->mtx_);
    while (block_offset_begin < block_offset_end_ && block_version->CheckDelete(block_offset_begin, begin_ts_)) {
        ++block_offset_begin;
    }
    BlockOffset row_idx;
    for (row_idx = block_offset_begin; row_idx < block_offset_end_; ++row_idx) {
        if (block_version->CheckDelete(row_idx, begin_ts_)) {
            break;
        }
    }
    visible_range = {block_offset_begin, row_idx};
    return block_offset_begin < row_idx;
}

Status NewTxn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    this->CheckTxn(db_name);
    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);

    auto append_command = MakeShared<WalCmdAppend>(db_name, table_name, input_block);
    {
        String db_id_str;
        String table_id_str;
        String table_key;
        Status status = this->GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
        if (!status.ok()) {
            return status;
        }
        append_command->db_id_str_ = db_id_str;
        append_command->table_id_str_ = table_id_str;
    }

    auto wal_command = static_pointer_cast<WalCmd>(append_command);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(append_command->ToString()));

    if (table_store->column_defs().empty()) {
        table_store->SetColumnDefs(db_name, table_name);
    }

    auto [err_msg, append_status] = table_store->Append(input_block);
    return Status::OK();
}

Status NewTxn::Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids) {
    this->CheckTxn(db_name);

    auto delete_command = MakeShared<WalCmdDelete>(db_name, table_name, row_ids);
    {
        String db_id_str;
        String table_id_str;
        String table_key;
        Status status = this->GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
        if (!status.ok()) {
            return status;
        }
        delete_command->db_id_str_ = db_id_str;
        delete_command->table_id_str_ = table_id_str;
    }

    auto wal_command = static_pointer_cast<WalCmd>(delete_command);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(delete_command->ToString()));

    NewTxnTableStore *table_store = this->GetNewTxnTableStore(table_name);
    auto [err_msg, delete_status] = table_store->Delete(row_ids);

    return delete_status;
}

Status NewTxn::AddNewSegment(const String &db_id_str,
                             const String &table_id_str,
                             SegmentID latest_segment_id,
                             SizeT segment_capacity,
                             const String &table_dir,
                             NewTxnTableStore *txn_table_store) {
    String lastest_block_id_key = KeyEncode::CatalogTableSegmentTagKey(db_id_str, table_id_str, latest_segment_id, LATEST_BLOCK_ID.data());
    Status status = kv_instance_->Put(lastest_block_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::AddNewBlock(const String &db_id_str,
                           const String &table_id_str,
                           SegmentID segment_id,
                           BlockID latest_block_id,
                           SizeT block_capacity,
                           const String &table_dir,
                           NewTxnTableStore *txn_table_store) {
    if (txn_table_store->column_defs().empty()) {
        UnrecoverableError("Column defs is empty.");
    }

    auto block_dir = MakeShared<String>(fmt::format("{}/seg_{}/block_{}", table_dir, segment_id, latest_block_id));
    for (SizeT column_idx = 0; column_idx < txn_table_store->column_defs().size(); ++column_idx) {
        Status status =
            this->AddNewColumn(db_id_str, table_id_str, segment_id, latest_block_id, column_idx, block_capacity, block_dir, txn_table_store);
        if (!status.ok()) {
            return status;
        }
    }

    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();

    auto version_file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir,
                                                             BlockVersion::FileName(),
                                                             block_capacity,
                                                             buffer_mgr->persistence_manager());
    BufferObj *buffer = buffer_mgr->AllocateBufferObject(std::move(version_file_worker));
    buffer->AddObjRc();

    {
        String block_lock_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, latest_block_id, "lock");
        Status status = new_catalog->AddBlockLock(std::move(block_lock_key));
        if (!status.ok()) {
            return status;
        }
    }
    {
        String block_dir_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, latest_block_id, "dir");
        Status status = kv_instance_->Put(block_dir_key, *block_dir);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::AddNewColumn(const String &db_id_str,
                            const String &table_id_str,
                            SegmentID segment_id,
                            BlockID block_id,
                            ColumnID column_idx,
                            SizeT block_capacity,
                            SharedPtr<String> block_dir,
                            NewTxnTableStore *txn_table_store) {
    const auto *col_def = txn_table_store->column_defs()[column_idx].get();
    String last_chunk_offset_key =
        KeyEncode::CatalogTableSegmentBlockColumnTagKey(db_id_str, table_id_str, segment_id, block_id, column_idx, "last_chunk_offset");
    Status status = kv_instance_->Put(last_chunk_offset_key, "0");
    if (!status.ok()) {
        return status;
    }

    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    auto filename = MakeShared<String>(fmt::format("{}.col", column_idx));
    SizeT total_data_size = block_capacity * col_def->type()->Size();
    auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                  MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                  block_dir,
                                                  filename,
                                                  total_data_size,
                                                  buffer_mgr->persistence_manager());

    BufferObj *buffer = buffer_mgr->AllocateBufferObject(std::move(file_worker));
    buffer->AddObjRc();

    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = MakeShared<String>(fmt::format("col_{}_out_0", column_idx));
        auto outline_file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir,
                                                             filename,
                                                             0, /*buffer_size*/
                                                             buffer_mgr->persistence_manager());
        [[maybe_unused]] BufferObj *outline_buffer = buffer_mgr->AllocateBufferObject(std::move(outline_file_worker));
        // outline_buffer->AddObjRc();
    }

    return Status::OK();
}

Status NewTxn::PrepareAppendInBlock(SegmentID segment_id,
                                    BlockID block_id,
                                    AppendState *append_state,
                                    SizeT block_capacity,
                                    SizeT block_row_count,
                                    SizeT &actual_append) {
    while (append_state->current_block_ < append_state->blocks_.size()) {
        DataBlock *input_block = append_state->blocks_[append_state->current_block_].get();
        SizeT to_append_rows = input_block->row_count() - append_state->current_block_offset_;
        if (to_append_rows == 0) {
            ++append_state->current_block_;
            append_state->current_block_offset_ = 0;
            continue;
        }

        actual_append = std::min(to_append_rows, block_capacity - block_row_count);
        AppendRange range(segment_id, block_id, block_row_count, actual_append, append_state->current_block_, append_state->current_block_offset_);
        append_state->append_ranges_.push_back(range);

        append_state->current_block_++;
        append_state->current_count_ += actual_append;
        append_state->current_block_offset_ += actual_append;
        break;
    }
    return Status::OK();
}

Status NewTxn::AppendInBlock(const String &db_id_str,
                             const String &table_id_str,
                             SegmentID segment_id,
                             BlockID block_id,
                             SizeT block_offset,
                             SizeT append_rows,
                             const DataBlock *input_block,
                             SizeT input_offset,
                             NewTxnTableStore *txn_table_store) {
    KVStore *kv_store = txn_mgr_->kv_store();
    String block_dir;
    {
        String block_dir_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "dir");
        // Get with no txn, because block_dir is unmutable.
        Status status = kv_store->Get(block_dir_key, block_dir);
        if (!status.ok()) {
            return status;
        }
    }

    BufferObj *version_buffer = nullptr;
    {
        String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + block_dir + "/" + String(BlockVersion::PATH);
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        version_buffer = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer == nullptr) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
        }
    }

    String block_lock_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "lock");
    NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;
    {
        SharedPtr<BlockLock> block_lock;
        Status status = new_catalog->GetBlockLock(block_lock_key, block_lock);
        if (!status.ok()) {
            return status;
        }
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        // append in column file
        // TODO: remove lock when append in column file
        for (SizeT column_idx = 0; column_idx < input_block->column_count(); ++column_idx) {
            const ColumnVector &column_vector = *input_block->column_vectors[column_idx];
            status = this->AppendInColumn(db_id_str,
                                          table_id_str,
                                          segment_id,
                                          block_id,
                                          column_idx,
                                          block_dir,
                                          block_offset,
                                          append_rows,
                                          column_vector,
                                          input_offset,
                                          txn_table_store);
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
Status NewTxn::AppendInColumn(const String &db_id_str,
                              const String &table_id_str,
                              SegmentID segment_id,
                              BlockID block_id,
                              SizeT column_idx,
                              String block_dir,
                              SizeT dest_offset,
                              SizeT append_rows,
                              const ColumnVector &source_vec,
                              SizeT source_offset,
                              NewTxnTableStore *txn_table_store) {
    KVStore *kv_store = txn_mgr_->kv_store();
    ColumnVector dest_vec;

    Status status = this->GetColumnVector(db_id_str,
                                          table_id_str,
                                          segment_id,
                                          block_id,
                                          column_idx,
                                          block_dir,
                                          dest_offset,
                                          ColumnVectorTipe::kReadWrite,
                                          txn_table_store,
                                          dest_vec);
    if (!status.ok()) {
        return status;
    }

    dest_vec.AppendWith(source_vec, source_offset, append_rows);
    LOG_TRACE(fmt::format("Append in column: db_id: {}, table_id: {}, segment_id: {}, block_id: {}, column_idx: {}, offset: {}, row_count: {}",
                          db_id_str,
                          table_id_str,
                          segment_id,
                          block_id,
                          column_idx,
                          dest_offset,
                          append_rows));

    if (VarBufferManager *var_buffer_mgr = dest_vec.buffer_->var_buffer_mgr(); var_buffer_mgr != nullptr) {
        // lock block outside, so update directly here
        SizeT last_chunk_size = var_buffer_mgr->TotalSize();
        String last_chunk_offset_key =
            KeyEncode::CatalogTableSegmentBlockColumnTagKey(db_id_str, table_id_str, segment_id, block_id, column_idx, "last_chunk_offset");
        String last_chunk_offset_str = fmt::format("{}", last_chunk_size);

        Status status = kv_store->Put(last_chunk_offset_key, last_chunk_offset_str);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::DeleteInBlock(const String &db_id_str,
                             const String &table_id_str,
                             SegmentID segment_id,
                             BlockID block_id,
                             const Vector<BlockOffset> &block_offsets) {
    KVStore *kv_store = txn_mgr_->kv_store();
    String block_dir;
    {
        String block_dir_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "dir");
        // Get with no txn, because block_dir is unmutable.
        Status status = kv_store->Get(block_dir_key, block_dir);
        if (!status.ok()) {
            return status;
        }
    }

    BufferObj *version_buffer = nullptr;
    {
        String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + block_dir + "/" + String(BlockVersion::PATH);
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        version_buffer = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer == nullptr) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
        }
    }

    String block_lock_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "lock");
    NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    {
        SharedPtr<BlockLock> block_lock;
        Status status = new_catalog->GetBlockLock(block_lock_key, block_lock);
        if (!status.ok()) {
            return status;
        }
        std::unique_lock<std::shared_mutex> lock(block_lock->mtx_);

        // delete in version file
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());
        for (BlockOffset block_offset : block_offsets) {
            block_version->Delete(block_offset, commit_ts);
        }
    }

    return Status::OK();
}

Status NewTxn::GetColumnVector(const String &db_id_str,
                               const String &table_id_str,
                               SegmentID segment_id,
                               BlockID block_id,
                               SizeT column_idx,
                               String block_dir,
                               SizeT row_count,
                               ColumnVectorTipe tipe,
                               NewTxnTableStore *txn_table_store,
                               ColumnVector &column_vector) {
    KVStore *kv_store = txn_mgr_->kv_store();
    const ColumnDef *col_def = txn_table_store->column_defs()[column_idx].get();
    BufferObj *buffer_obj = nullptr;
    {
        String col_filename = std::to_string(column_idx) + ".col";
        String col_filepath = InfinityContext::instance().config()->DataDir() + "/" + block_dir + "/" + col_filename;
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        buffer_obj = buffer_mgr->GetBufferObject(col_filepath);
        if (buffer_obj == nullptr) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", col_filepath));
        }
    }
    BufferObj *outline_buffer_obj = nullptr;
    [[maybe_unused]] SizeT last_chunk_offset = 0;
    {
        VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
        if (buffer_type == VectorBufferType::kVarBuffer) {
            String outline_filename = fmt::format("col_{}_out_0", column_idx);
            String outline_filepath = InfinityContext::instance().config()->DataDir() + "/" + block_dir + "/" + outline_filename;
            BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
            outline_buffer_obj = buffer_mgr->GetBufferObject(outline_filepath);
            if (outline_buffer_obj == nullptr) {
                return Status::BufferManagerError(fmt::format("Get outline buffer object failed: {}", outline_filepath));
            }

            String last_chunk_offset_key =
                KeyEncode::CatalogTableSegmentBlockColumnTagKey(db_id_str, table_id_str, segment_id, block_id, column_idx, "last_chunk_offset");
            String last_chunk_offset_str;
            // TODO: last_chunk_offset is not used here. check if get with no txn is allowed when it actually used.
            Status status = kv_store->Get(last_chunk_offset_key, last_chunk_offset_str);
            if (!status.ok()) {
                return status;
            }
            last_chunk_offset = std::stoul(last_chunk_offset_str);
        }
    }
    column_vector = ColumnVector(col_def->type());
    column_vector.Initialize(buffer_obj, outline_buffer_obj, row_count, tipe);

    return Status::OK();
}

Status NewTxn::GetBlockVisibleRange(const String &db_id_str,
                                    const String &table_id_str,
                                    SegmentID segment_id,
                                    BlockID block_id,
                                    NewTxnGetVisibleRangeState &state) {
    BufferObj *version_buffer = nullptr;
    KVStore *kv_store = txn_mgr_->kv_store();

    String block_dir;
    {
        String block_dir_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "dir");
        // Get with no txn, because block_dir is unmutable.
        Status status = kv_store->Get(block_dir_key, block_dir);
        if (!status.ok()) {
            return status;
        }
    }
    {
        String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + block_dir + "/" + String(BlockVersion::PATH);
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        version_buffer = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer == nullptr) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
        }
    }

    BufferHandle buffer_handle = version_buffer->Load();
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;

    SharedPtr<BlockLock> block_lock;
    {
        String block_lock_key = KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, segment_id, block_id, "lock");
        NewCatalog *new_catalog = infinity::InfinityContext::instance().storage()->new_catalog();
        Status status = new_catalog->GetBlockLock(block_lock_key, block_lock);
        if (!status.ok()) {
            return status;
        }
    }

    state.Init(std::move(block_lock), std::move(buffer_handle), begin_ts);
    return Status::OK();
}

Status NewTxn::PrepareCommitAppend(const WalCmdAppend *append_cmd) {
    const String &db_name = append_cmd->db_name_;
    const String &table_name = append_cmd->table_name_;

    // Get table ID
    String db_id_str;
    String table_id_str;
    String table_key;
    {
        Status status = GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
        if (!status.ok()) {
            return status;
        }
    }
    NewTxnTableStore *txn_table_store = txn_store_.GetNewTxnTableStore(table_name);
    String table_dir;
    {
        String table_dir_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, "dir");
        Status status = kv_instance_->Get(table_dir_key, table_dir);
        if (!status.ok()) {
            return status;
        }
    }

    AppendState *append_state = txn_table_store->GetAppendState();
    if (append_state == nullptr) {
        return Status::OK();
    }
    append_state->Finalize();
    if (append_state->Finished()) {
        return Status::OK();
    }

    SizeT block_capacity = DEFAULT_BLOCK_CAPACITY;
    SizeT segment_capacity = DEFAULT_SEGMENT_CAPACITY;

    SizeT cur_block_row_count = 0;
    SizeT old_block_row_count = 0;
    String block_row_cnt_key;

    SizeT cur_segment_row_count = 0;
    SizeT old_segment_row_count = 0;
    String segment_row_cnt_key;

    BlockID last_block_id = 0;
    BlockID old_last_block_id = 0;
    String last_block_id_key;

    SegmentID latest_segment_id = 0;
    SegmentID old_latest_segment_id = 0;
    String last_segment_id_key = KeyEncode::CatalogTableTagKey(db_id_str, table_id_str, LATEST_SEGMENT_ID.data());

    {
        String latest_segment_id_str;
        // TODO: not support parallel change last segment id here
        Status status = kv_instance_->GetForUpdate(last_segment_id_key, latest_segment_id_str);
        if (!status.ok()) {
            return status;
        }
        latest_segment_id = std::stoull(latest_segment_id_str);
        old_latest_segment_id = latest_segment_id;
    }

    bool init = false;
    while (true) {
        bool finished = append_state->Finished();
        if (finished && !init) {
            UnrecoverableError("Not init");
        }
        if (finished || cur_segment_row_count >= segment_capacity) {
            if (last_block_id != old_last_block_id) {
                String lastest_block_id_str = fmt::format("{}", last_block_id);
                Status status = kv_instance_->Put(last_block_id_key, lastest_block_id_str);
                if (!status.ok()) {
                    return status;
                }
            }
            if (cur_segment_row_count != old_segment_row_count) {
                String segment_row_cnt_str = fmt::format("{}", cur_segment_row_count);
                Status status = kv_instance_->Put(segment_row_cnt_key, segment_row_cnt_str);
                if (!status.ok()) {
                    return status;
                }
            }
        }
        if (finished || cur_segment_row_count >= segment_capacity || cur_block_row_count >= block_capacity) {
            if (cur_block_row_count != old_block_row_count) {
                String block_row_cnt_str = fmt::format("{}", cur_block_row_count);
                Status status = kv_instance_->Put(block_row_cnt_key, block_row_cnt_str);
                if (!status.ok()) {
                    return status;
                }
            }
        }
        if (finished) {
            break;
        }

        if ((!init && latest_segment_id == 0) || (init && cur_segment_row_count >= segment_capacity)) {
            Status status = this->AddNewSegment(db_id_str, table_id_str, latest_segment_id, segment_capacity, table_dir, txn_table_store);
            if (!status.ok()) {
                return status;
            }
            ++latest_segment_id;

            segment_row_cnt_key = KeyEncode::CatalogTableSegmentTagKey(db_id_str, table_id_str, latest_segment_id - 1, "row_cnt");
            cur_segment_row_count = 0;
            old_segment_row_count = 0;

            last_block_id_key = KeyEncode::CatalogTableSegmentTagKey(db_id_str, table_id_str, latest_segment_id - 1, LATEST_BLOCK_ID.data());
            last_block_id = 0;
            old_last_block_id = 0;

        } else if (!init) {
            {
                segment_row_cnt_key = KeyEncode::CatalogTableSegmentTagKey(db_id_str, table_id_str, latest_segment_id - 1, "row_cnt");
                String segment_row_cnt_str;
                Status status = kv_instance_->Get(segment_row_cnt_key, segment_row_cnt_str);
                if (!status.ok()) {
                    return status;
                }
                cur_segment_row_count = std::stoull(segment_row_cnt_str);
                old_segment_row_count = cur_segment_row_count;
            }
            {
                last_block_id_key = KeyEncode::CatalogTableSegmentTagKey(db_id_str, table_id_str, latest_segment_id - 1, LATEST_BLOCK_ID.data());
                String last_block_id_str;
                Status status = kv_instance_->Get(last_block_id_key, last_block_id_str);
                if (!status.ok()) {
                    return status;
                }
                last_block_id = std::stoull(last_block_id_str);
                old_last_block_id = last_block_id;
            }
        }

        if ((!init && last_block_id == 0) || (init && (cur_segment_row_count >= segment_capacity || cur_block_row_count >= block_capacity))) {
            Status status =
                this->AddNewBlock(db_id_str, table_id_str, latest_segment_id - 1, last_block_id, block_capacity, table_dir, txn_table_store);
            if (!status.ok()) {
                return status;
            }
            ++last_block_id;

            block_row_cnt_key =
                KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, latest_segment_id - 1, last_block_id - 1, "row_cnt");
            cur_block_row_count = 0;
            old_block_row_count = 0;
        } else if (!init) {
            {
                block_row_cnt_key =
                    KeyEncode::CatalogTableSegmentBlockTagKey(db_id_str, table_id_str, latest_segment_id - 1, last_block_id - 1, "row_cnt");
                String block_row_cnt_str;
                Status status = kv_instance_->Get(block_row_cnt_key, block_row_cnt_str);
                if (!status.ok()) {
                    return status;
                }
                cur_block_row_count = std::stoull(block_row_cnt_str);
                old_block_row_count = cur_block_row_count;
            }
        }

        init = true;

        SizeT actual_append = 0;
        {
            Status status = this->PrepareAppendInBlock(latest_segment_id - 1,
                                                       last_block_id - 1,
                                                       append_state,
                                                       block_capacity,
                                                       cur_block_row_count,
                                                       actual_append);
            if (!status.ok()) {
                return status;
            }
        }
        cur_block_row_count += actual_append;
        cur_segment_row_count += actual_append;
    }

    if (latest_segment_id != old_latest_segment_id) {
        String latest_segment_id_str = fmt::format("{}", latest_segment_id);
        Status status = kv_instance_->Put(last_segment_id_key, latest_segment_id_str);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CommitAppend(const WalCmdAppend *append_cmd) {
    const String &table_name = append_cmd->table_name_;
    const String &db_id_str = append_cmd->db_id_str_;
    const String &table_id_str = append_cmd->table_id_str_;

    NewTxnTableStore *txn_table_store = txn_store_.GetNewTxnTableStore(table_name);
    const AppendState *append_state = txn_table_store->GetAppendState();
    if (append_state == nullptr) {
        return Status::OK();
    }

    for (const AppendRange &range : append_state->append_ranges_) {
        const DataBlock *data_block = append_state->blocks_[range.data_block_idx_].get();
        Status status = this->AppendInBlock(db_id_str,
                                            table_id_str,
                                            range.segment_id_,
                                            range.block_id_,
                                            range.start_offset_,
                                            range.row_count_,
                                            data_block,
                                            range.data_block_offset_,
                                            txn_table_store);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CommitDelete(const WalCmdDelete *delete_cmd) {
    const String &table_name = delete_cmd->table_name_;
    const String &db_id_str = delete_cmd->db_id_str_;
    const String &table_id_str = delete_cmd->table_id_str_;

    NewTxnTableStore *txn_table_store = txn_store_.GetNewTxnTableStore(table_name);
    const DeleteState &delete_state = txn_table_store->GetDeleteStateRef();
    for (const auto &[segment_id, block_map] : delete_state.rows_) {
        for (const auto &[block_id, block_offsets] : block_map) {
            Status status = this->DeleteInBlock(db_id_str, table_id_str, segment_id, block_id, block_offsets);
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

} // namespace infinity

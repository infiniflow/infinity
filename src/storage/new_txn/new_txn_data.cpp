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
import data_type;
import column_meta;
import block_meta;
import segment_meta;
import table_meeta;
import table_index_meeta;

namespace infinity {

struct NewTxnCompactState {
    NewTxnCompactState(SegmentID segment_id, TableMeeta &table_meta) : new_segment_meta_(segment_id, table_meta, table_meta.kv_instance()) {
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            UnrecoverableError("Failed to get column defs");
        }
        column_cnt_ = column_defs_ptr->size();
    }

    SizeT column_cnt() const { return column_cnt_; }

    Status NextBlock(NewTxn *txn) {
        Status status;

        if (block_meta_) {
            status = block_meta_->SetRowCnt(cur_block_row_cnt_);
            if (!status.ok()) {
                return status;
            }
            block_meta_.reset();
        }

        BlockID block_id = 0;
        {
            status = new_segment_meta_.GetNextBlockID(block_id);
            if (!status.ok()) {
                return status;
            }
            status = new_segment_meta_.SetNextBlockID(block_id + 1);
            if (!status.ok()) {
                return status;
            }
        }
        status = txn->AddNewBlock(new_segment_meta_, block_id, block_meta_);
        if (!status.ok()) {
            return status;
        }
        cur_block_row_cnt_ = 0;

        column_vectors_.clear();
        column_vectors_.resize(column_cnt_);

        for (SizeT i = 0; i < column_cnt_; ++i) {
            ColumnMeta column_meta(i, *block_meta_, block_meta_->kv_instance());
            status = txn->GetColumnVector(column_meta, 0, ColumnVectorTipe::kReadWrite, column_vectors_[i]);
            if (!status.ok()) {
                return status;
            }
        }
        return status;
    }

    Status FinalizeBlock() {
        if (block_meta_) {
            Status status = block_meta_->SetRowCnt(cur_block_row_cnt_);
            if (!status.ok()) {
                return status;
            }
        }
        block_meta_.reset();
        return Status::OK();
    }

    Status Finalize() {
        if (block_meta_) {
            Status status = block_meta_->SetRowCnt(cur_block_row_cnt_);
            if (!status.ok()) {
                return status;
            }
        }
        Status status = new_segment_meta_.SetRowCnt(segment_row_cnt_);
        return status;
    }

    SegmentMeta new_segment_meta_;
    Optional<BlockMeta> block_meta_;
    SizeT segment_row_cnt_ = 0;
    BlockOffset cur_block_row_cnt_ = 0;
    Vector<ColumnVector> column_vectors_;
    SizeT column_cnt_ = 0;
};

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

Status NewTxn::Import(const String &db_name, const String &table_name, const Vector<SharedPtr<DataBlock>> &input_blocks) {
    this->CheckTxn(db_name);

    Status status;

    String db_id_str;
    String table_id_str;
    String table_key;
    status = this->GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
    if (!status.ok()) {
        return status;
    }
    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_.get());

    SegmentID segment_id = 0;
    status = table_meta.GetNextSegmentID(segment_id);
    if (!status.ok()) {
        return status;
    }
    status = table_meta.SetNextSegmentID(segment_id + 1);

    Optional<SegmentMeta> segment_meta;
    status = this->AddNewSegment(table_meta, segment_id, segment_meta);
    if (!status.ok()) {
        return status;
    }

    SizeT segment_row_cnt = 0;
    for (SizeT j = 0; j < input_blocks.size(); ++j) {
        const SharedPtr<DataBlock> &input_block = input_blocks[j];
        if (!input_block->Finalized()) {
            UnrecoverableError("Attempt to import unfinalized data block");
        }
        u32 row_cnt = input_block->row_count();

        BlockID block_id = 0;
        {
            status = segment_meta->GetNextBlockID(block_id);
            if (!status.ok()) {
                return status;
            }
            status = segment_meta->SetNextBlockID(block_id + 1);
            if (!status.ok()) {
                return status;
            }
        }
        Optional<BlockMeta> block_meta;
        status = this->AddNewBlock(*segment_meta, block_id, block_meta);
        if (!status.ok()) {
            return status;
        }

        if (j < input_blocks.size() - 1 && row_cnt != block_meta->block_capacity()) {
            UnrecoverableError("Attempt to import data block with different capacity");
        }

        for (SizeT i = 0; i < input_block->column_count(); ++i) {
            SharedPtr<ColumnVector> col = input_block->column_vectors[i];

            ColumnMeta column_meta(i, *block_meta, block_meta->kv_instance());

            BufferObj *buffer_obj = nullptr;
            BufferObj *outline_buffer_obj = nullptr;

            status = this->GetBufferObj(column_meta, buffer_obj, outline_buffer_obj);
            if (!status.ok()) {
                return status;
            }
            col->SetToCatalog(buffer_obj, outline_buffer_obj, ColumnVectorTipe::kReadWrite);

            buffer_obj->Save();
            if (outline_buffer_obj) {
                outline_buffer_obj->Save();
            }
        }

        status = block_meta->SetRowCnt(row_cnt);
        if (!status.ok()) {
            return status;
        }
        segment_row_cnt += row_cnt;
    }
    status = segment_meta->SetRowCnt(segment_row_cnt);
    if (!status.ok()) {
        return status;
    }

    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_names_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
        const String &index_id_str = (*index_id_strs_ptr)[i];
        const String &index_name = (*index_names_ptr)[i];
        TableIndexMeeta table_index_meta(index_id_str, table_meta, table_meta.kv_instance());

        status = this->PopulateIndex(db_name, table_name, index_name, table_index_meta, *segment_meta);
        if (!status.ok()) {
            return status;
        }
    }

    auto import_command = MakeShared<WalCmdImport>(db_name, table_name, WalSegmentInfo(*segment_meta));
    wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(import_command));
    txn_context_ptr_->AddOperation(MakeShared<String>(import_command->ToString()));
    import_command->db_id_str_ = db_id_str;
    import_command->table_id_str_ = table_id_str;

    return Status::OK();
}

Status NewTxn::Append(const String &db_name, const String &table_name, const SharedPtr<DataBlock> &input_block) {
    this->CheckTxn(db_name);
    NewTxnTableStore1 *table_store = nullptr;

    auto append_command = MakeShared<WalCmdAppend>(db_name, table_name, input_block);
    String db_id_str;
    String table_id_str;
    {
        String table_key;
        Status status = this->GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
        if (!status.ok()) {
            return status;
        }
        append_command->db_id_str_ = db_id_str;
        append_command->table_id_str_ = table_id_str;
        table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    }

    auto wal_command = static_pointer_cast<WalCmd>(append_command);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(append_command->ToString()));

    {
        TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_.get());

        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs{nullptr};
        {
            auto [col_defs, status] = table_meta.GetColumnDefs();
            if (!status.ok()) {
                return status;
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
    }

    auto append_status = table_store->Append(input_block);
    if (!append_status.ok()) {
        return append_status;
    }
    return Status::OK();
}

Status NewTxn::Delete(const String &db_name, const String &table_name, const Vector<RowID> &row_ids) {
    this->CheckTxn(db_name);
    NewTxnTableStore1 *table_store = nullptr;

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
        table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    }

    auto wal_command = static_pointer_cast<WalCmd>(delete_command);
    wal_entry_->cmds_.push_back(wal_command);
    txn_context_ptr_->AddOperation(MakeShared<String>(delete_command->ToString()));

    auto delete_status = table_store->Delete(row_ids);
    if (!delete_status.ok()) {
        return delete_status;
    }

    return Status::OK();
}

Status NewTxn::Compact(const String &db_name, const String &table_name) {
    Status status;

    this->CheckTxn(db_name);

    String db_id_str;
    String table_id_str;
    {
        String table_key;
        Status status = this->GetTableID(db_name, table_name, table_key, table_id_str, db_id_str);
        if (!status.ok()) {
            return status;
        }
    }
    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_.get());

    SegmentID new_segment_id = 0;
    status = table_meta.GetNextSegmentID(new_segment_id);
    if (!status.ok()) {
        return status;
    }
    status = table_meta.SetNextSegmentID(new_segment_id + 1);

    NewTxnCompactState compact_state(new_segment_id, table_meta);

    SharedPtr<Vector<SegmentID>> segment_ids_ptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs();
    if (!status.ok()) {
        return status;
    }

    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta, table_meta.kv_instance());

        SharedPtr<Vector<BlockID>> block_ids_ptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs();
        if (!status.ok()) {
            return status;
        }

        for (BlockID block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());
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

    Vector<String> *index_id_strs_ptr = nullptr;
    Vector<String> *index_name_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr, &index_name_ptr);
    if (!status.ok()) {
        return status;
    }

    for (SizeT i = 0; i < index_id_strs_ptr->size(); ++i) {
        const String &index_id_str = (*index_id_strs_ptr)[i];
        const String &index_name = (*index_name_ptr)[i];
        TableIndexMeeta table_index_meta(index_id_str, table_meta, table_meta.kv_instance());

        status = this->PopulateIndex(db_name, table_name, index_name, table_index_meta, compact_state.new_segment_meta_);
        if (!status.ok()) {
            return status;
        }
    }

    status = table_meta.SetSegmentIDs(Vector<SegmentID>{new_segment_id});
    if (!status.ok()) {
        return status;
    }
    {
        Vector<SegmentID> deprecated_segment_ids = *segment_ids_ptr;
        Vector<WalSegmentInfo> segment_infos;
        segment_infos.emplace_back(compact_state.new_segment_meta_);
        auto compact_command = MakeShared<WalCmdCompact>(db_name, table_name, std::move(segment_infos), std::move(deprecated_segment_ids));
        wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(compact_command));
        txn_context_ptr_->AddOperation(MakeShared<String>(compact_command->ToString()));
        compact_command->db_id_str_ = db_id_str;
        compact_command->table_id_str_ = table_id_str;
    }

    return Status::OK();
}

Status NewTxn::AddNewSegment(TableMeeta &table_meta, SegmentID segment_id, Optional<SegmentMeta> &segment_meta) {
    {
        Status status = table_meta.AddSegmentID(segment_id);
        if (!status.ok()) {
            return status;
        }
    }
    segment_meta.emplace(segment_id, table_meta, table_meta.kv_instance());
    {
        Status status = segment_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::AddNewBlock(SegmentMeta &segment_meta, BlockID block_id, Optional<BlockMeta> &block_meta) {
    {
        Status status = segment_meta.AddBlockID(block_id);
        if (!status.ok()) {
            return status;
        }
    }
    block_meta.emplace(block_id, segment_meta, segment_meta.kv_instance());
    {
        Status status = block_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<String> block_dir_ptr = block_meta->GetBlockDir();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    BufferObj *version_buffer = nullptr;
    {
        auto version_file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                 MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                 block_dir_ptr,
                                                                 BlockVersion::FileName(),
                                                                 block_meta->block_capacity(),
                                                                 buffer_mgr->persistence_manager());
        version_buffer = buffer_mgr->AllocateBufferObject(std::move(version_file_worker));
        version_buffer->AddObjRc();
    }
    SizeT column_num = 0;
    {
        TableMeeta &table_meta = segment_meta.table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        column_num = column_defs_ptr->size();
    }
    for (SizeT column_idx = 0; column_idx < column_num; ++column_idx) {
        [[maybe_unused]] Optional<ColumnMeta> column_meta;
        Status status = this->AddNewBlockColumn(*block_meta, column_idx, column_meta);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::AddNewBlockColumn(BlockMeta &block_meta, SizeT column_idx, Optional<ColumnMeta> &column_meta) {
    const ColumnDef *col_def = nullptr;
    {
        TableMeeta &table_meta = block_meta.segment_meta().table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_idx].get();
    }

    column_meta.emplace(column_idx, block_meta, block_meta.kv_instance());
    {
        Status status = column_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    BufferObj *buffer = nullptr;
    {
        auto filename = MakeShared<String>(fmt::format("{}.col", column_idx));
        SizeT total_data_size = block_meta.block_capacity() * col_def->type()->Size();
        auto file_worker = MakeUnique<DataFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                      MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                      block_dir_ptr,
                                                      filename,
                                                      total_data_size,
                                                      buffer_mgr->persistence_manager());
        buffer = buffer_mgr->AllocateBufferObject(std::move(file_worker));
        buffer->AddObjRc();
    }
    VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
    if (buffer_type == VectorBufferType::kVarBuffer) {
        auto filename = MakeShared<String>(fmt::format("col_{}_out_0", column_idx));
        auto outline_file_worker = MakeUnique<VarFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir_ptr,
                                                             filename,
                                                             0, /*buffer_size*/
                                                             buffer_mgr->persistence_manager());
        [[maybe_unused]] BufferObj *outline_buffer = buffer_mgr->AllocateBufferObject(std::move(outline_file_worker));
        // outline_buffer->AddObjRc();
    }
    return Status::OK();
}

Status NewTxn::PrepareAppendInBlock(BlockMeta &block_meta, AppendState *append_state, bool &block_full, bool &segment_full) {
    while (append_state->current_block_ < append_state->blocks_.size()) {
        DataBlock *input_block = append_state->blocks_[append_state->current_block_].get();
        SizeT to_append_rows = input_block->row_count() - append_state->current_block_offset_;
        if (to_append_rows == 0) {
            ++append_state->current_block_;
            append_state->current_block_offset_ = 0;
            continue;
        }
        SizeT block_row_count = 0;
        {
            Status status = block_meta.GetRowCnt(block_row_count);
            if (!status.ok()) {
                return status;
            }
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

            block_meta.SetRowCnt(block_row_count + actual_append);
        }

        block_full = block_row_count + actual_append >= block_meta.block_capacity();

        SizeT segment_row_count = 0;
        {
            Status status = block_meta.segment_meta().GetRowCnt(segment_row_count);
            if (!status.ok()) {
                return status;
            }
        }
        if (actual_append) {
            block_meta.segment_meta().SetRowCnt(segment_row_count + actual_append);
        }
        segment_full = segment_row_count + actual_append >= block_meta.segment_meta().segment_capacity();

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
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    BufferObj *version_buffer = nullptr;
    {
        String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + String(BlockVersion::PATH);
        version_buffer = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer == nullptr) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
        }
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

        // append in column file
        for (SizeT column_idx = 0; column_idx < input_block->column_count(); ++column_idx) {
            const ColumnVector &column_vector = *input_block->column_vectors[column_idx];
            ColumnMeta column_meta(column_idx, block_meta, block_meta.kv_instance());
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
        Status status = this->GetColumnVector(column_meta, dest_offset, ColumnVectorTipe::kReadWrite, dest_vec);
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

Status NewTxn::DeleteInBlock(BlockMeta &block_meta, const Vector<BlockOffset> &block_offsets) {
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
        for (BlockOffset block_offset : block_offsets) {
            block_version->Delete(block_offset, commit_ts);
        }
    }
    return Status::OK();
}

Status NewTxn::CompactBlock(BlockMeta &block_meta, NewTxnCompactState &compact_state) {
    Status status;

    NewTxnGetVisibleRangeState range_state;
    status = this->GetBlockVisibleRange(block_meta, range_state);
    if (!status.ok()) {
        return status;
    }

    SizeT block_row_cnt = range_state.block_offset_end();

    SizeT column_cnt = compact_state.column_cnt();
    Vector<ColumnVector> column_vectors;
    column_vectors.resize(column_cnt);
    for (SizeT column_id = 0; column_id < column_cnt; ++column_id) {
        ColumnMeta column_meta(column_id, block_meta, block_meta.kv_instance());

        status = this->GetColumnVector(column_meta, block_row_cnt, ColumnVectorTipe::kReadOnly, column_vectors[column_id]);
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
                status = compact_state.NextBlock(this);
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
                status = compact_state.NextBlock(this);
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

Status NewTxn::GetBufferObj(ColumnMeta &column_meta, BufferObj *&buffer_obj, BufferObj *&outline_buffer_obj) {
    SharedPtr<String> block_dir_ptr = column_meta.block_meta().GetBlockDir();
    const ColumnDef *col_def = nullptr;
    {
        TableMeeta &table_meta = column_meta.block_meta().segment_meta().table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_meta.column_idx()].get();
    }

    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    {
        String col_filename = std::to_string(column_meta.column_idx()) + ".col";
        String col_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + col_filename;
        buffer_obj = buffer_mgr->GetBufferObject(col_filepath);
        if (buffer_obj == nullptr) {
            return Status::BufferManagerError(fmt::format("Get buffer object failed: {}", col_filepath));
        }
    }
    [[maybe_unused]] SizeT chunk_offset = 0;
    {
        VectorBufferType buffer_type = ColumnVector::GetVectorBufferType(*col_def->type());
        if (buffer_type == VectorBufferType::kVarBuffer) {
            String outline_filename = fmt::format("col_{}_out_0", column_meta.column_idx());
            String outline_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + outline_filename;
            BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
            outline_buffer_obj = buffer_mgr->GetBufferObject(outline_filepath);
            if (outline_buffer_obj == nullptr) {
                return Status::BufferManagerError(fmt::format("Get outline buffer object failed: {}", outline_filepath));
            }
            {
                Status status = column_meta.GetChunkOffset(chunk_offset);
                if (!status.ok()) {
                    return status;
                }
            }
        }
    }
    return Status::OK();
}

Status NewTxn::GetColumnVector(ColumnMeta &column_meta, SizeT row_count, ColumnVectorTipe tipe, ColumnVector &column_vector) {
    const ColumnDef *col_def = nullptr;
    {
        TableMeeta &table_meta = column_meta.block_meta().segment_meta().table_meta();
        auto [column_defs_ptr, status] = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        col_def = (*column_defs_ptr)[column_meta.column_idx()].get();
    }

    BufferObj *buffer_obj = nullptr;
    BufferObj *outline_buffer_obj = nullptr;
    Status status = this->GetBufferObj(column_meta, buffer_obj, outline_buffer_obj);
    if (!status.ok()) {
        return status;
    }

    column_vector = ColumnVector(col_def->type());
    column_vector.Initialize(buffer_obj, outline_buffer_obj, row_count, tipe);
    return Status::OK();
}

Status NewTxn::GetBlockVisibleRange(BlockMeta &block_meta, NewTxnGetVisibleRangeState &state) {
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

    BufferHandle buffer_handle = version_buffer->Load();
    TxnTimeStamp begin_ts = txn_context_ptr_->begin_ts_;
    SharedPtr<BlockLock> block_lock;
    {
        Status status = block_meta.GetBlockLock(block_lock);
        if (!status.ok()) {
            return status;
        }
    }
    state.Init(std::move(block_lock), std::move(buffer_handle), begin_ts);
    return Status::OK();
}

Status NewTxn::CommitImport(const WalCmdImport *import_cmd) {
    const String &db_id_str = import_cmd->db_id_str_;
    const String &table_id_str = import_cmd->table_id_str_;

    const WalSegmentInfo &segment_info = import_cmd->segment_info_;

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_.get());
    SegmentMeta segment_meta(segment_info.segment_id_, table_meta, table_meta.kv_instance());

    Status status = this->CommitSegmentVersion(segment_info, segment_meta);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewTxn::CommitAppend(const WalCmdAppend *append_cmd) {
    const String &db_id_str = append_cmd->db_id_str_;
    const String &table_id_str = append_cmd->table_id_str_;

    NewTxnTableStore1 *txn_table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    AppendState *append_state = txn_table_store->append_state();
    append_state->Finalize();
    if (append_state->Finished()) {
        // FIXME append_state will merge all appended blocks data. If append more than one block, this branch will be executed.
        return Status::OK();
    }

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_.get());

    Status status;

    SegmentID next_segment_id = 0;

    status = table_meta.GetNextSegmentID(next_segment_id);
    if (!status.ok()) {
        return status;
    }
    Optional<SegmentMeta> segment_meta;
    if (next_segment_id == 0) {
        SegmentID segment_id = 0;

        status = this->AddNewSegment(table_meta, segment_id, segment_meta);
        if (!status.ok()) {
            return status;
        }
        status = table_meta.SetNextSegmentID(segment_id + 1);
        if (!status.ok()) {
            return status;
        }
    } else {
        SegmentID segment_id = next_segment_id - 1;
        segment_meta.emplace(segment_id, table_meta, *kv_instance_.get());
    }
    BlockID next_block_id = 0;
    status = segment_meta->GetNextBlockID(next_block_id);
    if (!status.ok()) {
        return status;
    }
    Optional<BlockMeta> block_meta;
    if (next_block_id == 0) {
        BlockID block_id = 0;
        status = this->AddNewBlock(*segment_meta, block_id, block_meta);
        if (!status.ok()) {
            return status;
        }
        status = segment_meta->SetNextBlockID(block_id + 1);
        if (!status.ok()) {
            return status;
        }
    } else {
        BlockID block_id = next_block_id - 1;
        block_meta.emplace(block_id, segment_meta.value(), *kv_instance_.get());
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
            SegmentID segment_id = next_segment_id;
            status = this->AddNewSegment(table_meta, segment_id, segment_meta);
            if (!status.ok()) {
                return status;
            }
            ++next_segment_id;
            status = table_meta.SetNextSegmentID(next_segment_id);
            if (!status.ok()) {
                return status;
            }
            BlockID block_id = 0;
            status = this->AddNewBlock(*segment_meta, block_id, block_meta);
            if (!status.ok()) {
                return status;
            }
            next_block_id = 1;
            status = segment_meta->SetNextBlockID(next_block_id);
            if (!status.ok()) {
                return status;
            }
        } else if (block_full) {
            BlockID block_id = next_block_id;
            status = this->AddNewBlock(*segment_meta, block_id, block_meta);
            if (!status.ok()) {
                return status;
            }
            ++next_block_id;
            status = segment_meta->SetNextBlockID(next_block_id);
            if (!status.ok()) {
                return status;
            }
        }
    }

    return Status::OK();
}

Status NewTxn::PostCommitAppend(const WalCmdAppend *append_cmd) {
    KVStore *kv_store = txn_mgr_->kv_store();
    UniquePtr<KVInstance> kv_instance = kv_store->GetInstance();

    const String &db_id_str = append_cmd->db_id_str_;
    const String &table_id_str = append_cmd->table_id_str_;

    NewTxnTableStore1 *txn_table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    const AppendState *append_state = txn_table_store->append_state();
    if (append_state == nullptr) {
        return Status::OK();
    }
    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance);
    Optional<SegmentMeta> segment_meta;
    Optional<BlockMeta> block_meta;
    for (const AppendRange &range : append_state->append_ranges_) {
        const DataBlock *data_block = append_state->blocks_[range.data_block_idx_].get();
        SegmentID segment_id = range.segment_id_;
        if (!segment_meta || segment_meta->segment_id() != segment_id) {
            segment_meta.emplace(segment_id, table_meta, *kv_instance);
            block_meta.reset();
        }
        BlockID block_id = range.block_id_;
        if (!block_meta || block_meta->block_id() != block_id) {
            block_meta.emplace(block_id, segment_meta.value(), *kv_instance);
        }
        Status status = this->AppendInBlock(*block_meta, range.start_offset_, range.row_count_, data_block, range.data_block_offset_);
        if (!status.ok()) {
            return status;
        }
    }
    Vector<String> *index_id_strs = nullptr;
    {
        Status status = table_meta.GetIndexIDs(index_id_strs, nullptr);
        if (!status.ok()) {
            return status;
        }
    }
    for (SizeT i = 0; i < index_id_strs->size(); ++i) {
        const String &index_id_str = (*index_id_strs)[i];
        TableIndexMeeta table_index_meta(index_id_str, table_meta, *kv_instance);
        Status status = this->AppendIndex(table_index_meta, append_state);
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = kv_instance->Commit();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::PostCommitDelete(const WalCmdDelete *delete_cmd) {
    KVStore *kv_store = txn_mgr_->kv_store();
    UniquePtr<KVInstance> kv_instance = kv_store->GetInstance();

    const String &db_id_str = delete_cmd->db_id_str_;
    const String &table_id_str = delete_cmd->table_id_str_;

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance);

    Optional<SegmentMeta> segment_meta;
    Optional<BlockMeta> block_meta;

    NewTxnTableStore1 *txn_table_store = txn_store_.GetNewTxnTableStore1(db_id_str, table_id_str);
    if (txn_table_store == nullptr) {
        return Status::OK();
    }
    const DeleteState &delete_state = txn_table_store->delete_state();
    for (const auto &[segment_id, block_map] : delete_state.rows_) {
        if (!segment_meta || segment_id != segment_meta->segment_id()) {
            segment_meta.emplace(segment_id, table_meta, *kv_instance);
            block_meta.reset();
        }
        for (const auto &[block_id, block_offsets] : block_map) {
            if (!block_meta || block_id != block_meta->block_id()) {
                block_meta.emplace(block_id, segment_meta.value(), *kv_instance);
            }
            Status status = this->DeleteInBlock(*block_meta, block_offsets);
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        Status status = kv_instance->Commit();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::CommitCompact(const WalCmdCompact *compact_cmd) {
    const String &db_id_str = compact_cmd->db_id_str_;
    const String &table_id_str = compact_cmd->table_id_str_;

    const Vector<WalSegmentInfo> &segment_infos = compact_cmd->new_segment_infos_;
    if (segment_infos.empty()) {
        return Status::OK();
    }
    if (segment_infos.size() > 1) {
        UnrecoverableError("Not implemented");
    }
    const WalSegmentInfo &segment_info = segment_infos[0];

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_.get());
    SegmentMeta segment_meta(segment_info.segment_id_, table_meta, table_meta.kv_instance());

    Status status = this->CommitSegmentVersion(segment_info, segment_meta);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewTxn::CommitSegmentVersion(const WalSegmentInfo &segment_info, SegmentMeta &segment_meta) {
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    for (const WalBlockInfo &block_info : segment_info.block_infos_) {
        BlockMeta block_meta(block_info.block_id_, segment_meta, segment_meta.kv_instance());
        SharedPtr<String> block_dir_ptr = block_meta.GetBlockDir();

        BufferObj *version_buffer = nullptr;
        {
            String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + String(BlockVersion::PATH);
            version_buffer = buffer_mgr->GetBufferObject(version_filepath);
            if (version_buffer == nullptr) {
                return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
            }
        }
        BufferHandle buffer_handle = version_buffer->Load();
        auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());

        block_version->Append(commit_ts, block_info.row_count_);
        version_buffer->Save(VersionFileWorkerSaveCtx(commit_ts));
    }

    return Status::OK();
}

} // namespace infinity

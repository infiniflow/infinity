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

#include <cassert>
#include <vector>

module new_txn;

import chunk_index_meta;
import segment_index_meta;
import table_index_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import logger;
import infinity_context;
import buffer_manager;
import third_party;
import infinity_exception;
import secondary_index_file_worker;
import ivf_index_file_worker;
import raw_file_worker;
import column_def;
import internal_types;
import file_worker;
import create_index_info;
import buffer_obj;
import mem_index;
import wal_entry;
import kv_code;
import secondary_index_in_mem;
import secondary_index_data;
import default_values;
import ivf_index_data_in_mem;
import ivf_index_data;
import memory_indexer;
import index_defines;
import index_full_text;
import column_index_reader;
import column_index_merger;

namespace infinity {

namespace {

String IndexFileName(SegmentID segment_id, ChunkID chunk_id) { return fmt::format("seg{}_chunk{}.idx", segment_id, chunk_id); }

} // namespace

Status NewTxn::DumpMemIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id) {
    String db_id_str;
    String table_id_str;
    String index_id_str;
    String index_key;
    {
        Status status = this->GetIndexID(db_name, table_name, index_name, index_key, index_id_str, table_id_str, db_id_str);
        if (!status.ok()) {
            return status;
        }
    }

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_);
    TableIndexMeeta table_index_meta(index_id_str, table_meta, table_meta.kv_instance());
    SegmentIndexMeta segment_index_meta(segment_id, table_index_meta, table_index_meta.kv_instance());

    ChunkID new_chunk_id = 0;
    {
        Status status = this->DumpMemIndexInner(segment_index_meta, new_chunk_id);
        if (!status.ok()) {
            return status;
        }
    }
    {
        ChunkIndexMeta chunk_index_meta(new_chunk_id, segment_index_meta, segment_index_meta.kv_instance());
        Status status = this->AddChunkWal(db_name, table_name, index_name, chunk_index_meta, {}, true);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::OptimizeIndex(const String &db_name, const String &table_name, const String &index_name, SegmentID segment_id) {
    String db_id_str;
    String table_id_str;
    String index_id_str;
    String index_key;
    {
        Status status = this->GetIndexID(db_name, table_name, index_name, index_key, index_id_str, table_id_str, db_id_str);
        if (!status.ok()) {
            return status;
        }
    }

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_);
    TableIndexMeeta table_index_meta(index_id_str, table_meta, table_meta.kv_instance());
    SegmentIndexMeta segment_index_meta(segment_id, table_index_meta, table_index_meta.kv_instance());

    Vector<ChunkID> *old_chunk_ids_ptr = nullptr;
    {
        Status status = segment_index_meta.GetChunkIDs(old_chunk_ids_ptr);
        if (!status.ok()) {
            return status;
        }
    }
    RowID base_rowid;
    u32 row_cnt = 0;
    Vector<Pair<u32, BufferObj *>> old_buffers;
    String base_name;
    SharedPtr<IndexBase> index_base;
    {
        Status status = table_index_meta.GetIndexDef(index_base);
        if (!status.ok()) {
            return status;
        }
    }
    if (index_base->index_type_ == IndexType::kFullText) {
        Status status = this->OptimizeFtIndex(index_base, segment_index_meta, base_rowid, row_cnt, base_name);
        if (!status.ok()) {
            return status;
        }
    } else {
        base_rowid = RowID(segment_id, 0);
        RowID last_rowid = base_rowid;
        ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
        for (ChunkID old_chunk_id : *old_chunk_ids_ptr) {
            ChunkIndexMeta old_chunk_meta(old_chunk_id, segment_index_meta, segment_index_meta.kv_instance());
            {
                Status status = old_chunk_meta.GetChunkInfo(chunk_info_ptr);
                if (!status.ok()) {
                    return status;
                }
            }
            if (last_rowid != chunk_info_ptr->base_row_id_) {
                UnrecoverableError("OptimizeIndex: base_row_id is not continuous");
            }
            last_rowid.segment_offset_ += chunk_info_ptr->row_cnt_;
            row_cnt += chunk_info_ptr->row_cnt_;

            BufferObj *buffer_obj = nullptr;
            {
                Status status = this->GetChunkIndex(old_chunk_meta, buffer_obj);
                if (!status.ok()) {
                    return status;
                }
            }
            old_buffers.emplace_back(chunk_info_ptr->row_cnt_, buffer_obj);
        }
    }
    Vector<ChunkID> deprecate_ids = *old_chunk_ids_ptr;
    ChunkID chunk_id = 0;
    {
        Status status = segment_index_meta.GetNextChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            return status;
        }
    }
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        Status status = this->AddNewChunkIndex(segment_index_meta, chunk_id, base_rowid, row_cnt, base_name, chunk_index_meta, buffer_obj);
        if (!status.ok()) {
            return status;
        }
    }
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            BufferHandle buffer_handle = buffer_obj->Load();
            auto *data_ptr = static_cast<SecondaryIndexData *>(buffer_handle.GetDataMut());
            data_ptr->InsertMergeData(old_buffers);
            break;
        }
        case IndexType::kFullText: {
            // Skip here. merge finished in `OptimizeFtIndex`
            break;
        }
        case IndexType::kIVF: {
            SegmentMeta segment_meta(segment_id, table_meta, table_meta.kv_instance());
            SharedPtr<ColumnDef> column_def;
            {
                auto [col_def, status] = table_index_meta.GetColumnDef();
                if (!status.ok()) {
                    return status;
                }
                column_def = std::move(col_def);
            }

            BufferHandle buffer_handle = buffer_obj->Load();
            auto *data_ptr = static_cast<IVFIndexInChunk *>(buffer_handle.GetDataMut());
            data_ptr->BuildIVFIndex(this, segment_meta, row_cnt, column_def);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    {
        Vector<ChunkID> chunk_ids = {chunk_id};
        Status status = segment_index_meta.SetChunkIDs(chunk_ids);
        if (!status.ok()) {
            return status;
        }
    }
    {
        Vector<WalChunkIndexInfo> chunk_infos;
        chunk_infos.emplace_back(*chunk_index_meta);
        auto dump_cmd = MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id, std::move(chunk_infos), std::move(deprecate_ids));

        wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(dump_cmd));
        txn_context_ptr_->AddOperation(MakeShared<String>(dump_cmd->ToString()));
    }
    return Status::OK();
}

Status NewTxn::AddNewSegmentIndex(TableIndexMeeta &table_index_meta, SegmentID segment_id, Optional<SegmentIndexMeta> &segment_index_meta) {
    {
        Status status = table_index_meta.AddSegmentID(segment_id);
        if (!status.ok()) {
            return status;
        }
    }
    {
        segment_index_meta.emplace(segment_id, table_index_meta, table_index_meta.kv_instance());
        Status status = segment_index_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::AddNewChunkIndex(SegmentIndexMeta &segment_index_meta,
                                ChunkID chunk_id,
                                RowID base_row_id,
                                SizeT row_count,
                                const String &base_name,
                                Optional<ChunkIndexMeta> &chunk_index_meta,
                                BufferObj *&buffer_obj) {
    TableIndexMeeta &table_index_meta = segment_index_meta.table_index_meta();
    SegmentID segment_id = segment_index_meta.segment_id();

    SharedPtr<IndexBase> index_base;
    {
        Status status = table_index_meta.GetIndexDef(index_base);
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<ColumnDef> column_def;
    {
        auto [col_def, status] = table_index_meta.GetColumnDef();
        if (!status.ok()) {
            return status;
        }
        column_def = std::move(col_def);
    }

    SharedPtr<String> index_dir = table_index_meta.GetTableIndexDir();
    ChunkIndexMetaInfo chunk_info;
    {
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        switch (index_base->index_type_) {
            case IndexType::kSecondary: {
                auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
                auto index_file_worker = MakeUnique<SecondaryIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                              MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                              index_dir,
                                                                              std::move(secondary_index_file_name),
                                                                              index_base,
                                                                              column_def,
                                                                              row_count,
                                                                              buffer_mgr->persistence_manager());
                buffer_obj = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kFullText: {
                auto column_length_file_name = MakeShared<String>(base_name + LENGTH_SUFFIX);
                auto index_file_worker = MakeUnique<RawFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                   MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                   index_dir,
                                                                   std::move(column_length_file_name),
                                                                   row_count * sizeof(u32),
                                                                   buffer_mgr->persistence_manager());
                buffer_obj = buffer_mgr->GetBufferObject(std::move(index_file_worker));
                break;
            }
            case IndexType::kIVF: {
                auto ivf_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
                auto index_file_worker = MakeUnique<IVFIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                        MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                        index_dir,
                                                                        std::move(ivf_index_file_name),
                                                                        index_base,
                                                                        column_def,
                                                                        buffer_mgr->persistence_manager());
                buffer_obj = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
                break;
            }
            default: {
                UnrecoverableError("Not implemented yet");
            }
        }
        if (buffer_obj == nullptr) {
            return Status::BufferManagerError("AllocateBufferObject failed");
        }
        buffer_obj->AddObjRc();
    }
    chunk_info.base_name_ = base_name;
    chunk_info.base_row_id_ = base_row_id;
    chunk_info.row_cnt_ = row_count;
    {
        chunk_index_meta.emplace(chunk_id, segment_index_meta, segment_index_meta.kv_instance());
        Status status = chunk_index_meta->InitSet(chunk_info);
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = segment_index_meta.AddChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::AppendIndex(TableIndexMeeta &table_index_meta, const AppendState *append_state) {
    SharedPtr<IndexBase> index_def;
    {
        Status status = table_index_meta.GetIndexDef(index_def);
        if (!status.ok()) {
            return status;
        }
    }
    ColumnID column_id = 0;
    {
        auto [column_def, status] = table_index_meta.table_meta().GetColumnDefByColumnName(index_def->column_name());
        if (!status.ok()) {
            return status;
        }
        column_id = column_def->id();
    }
    Optional<SegmentMeta> segment_meta;
    Optional<SegmentIndexMeta> segment_index_meta;
    Optional<BlockMeta> block_meta;
    Optional<ColumnMeta> column_meta;
    BlockOffset cur_offset = 0;
    BlockOffset cur_row_cnt = 0;

    auto append_in_column = [&]() {
        ColumnVector col;
        {
            Status status = this->GetColumnVector(*column_meta, cur_offset + cur_row_cnt, ColumnVectorTipe::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
        }
        SegmentOffset block_offset = block_meta->block_capacity() * block_meta->block_id() + cur_offset;
        RowID base_row_id(segment_meta->segment_id(), block_offset);
        {
            Status status = this->AppendMemIndex(*segment_index_meta, base_row_id, col, cur_offset, cur_row_cnt);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };

    for (const AppendRange &range : append_state->append_ranges_) {
        if (!segment_meta || segment_meta->segment_id() != range.segment_id_) {
            segment_meta.emplace(range.segment_id_, table_index_meta.table_meta(), table_index_meta.kv_instance());

            {
                Vector<SegmentID> *segment_ids_ptr = nullptr;
                Status status = table_index_meta.GetSegmentIDs(segment_ids_ptr);
                if (!status.ok()) {
                    return status;
                }
                auto iter = std::find(segment_ids_ptr->begin(), segment_ids_ptr->end(), range.segment_id_);
                if (iter == segment_ids_ptr->end()) {
                    status = this->AddNewSegmentIndex(table_index_meta, range.segment_id_, segment_index_meta);
                    if (!status.ok()) {
                        return status;
                    }
                } else {
                    segment_index_meta.emplace(range.segment_id_, table_index_meta, table_index_meta.kv_instance());
                }
            }
            block_meta.reset();
        }
        if (!block_meta || block_meta->block_id() != range.block_id_) {
            if (block_meta) {
                append_in_column();
            }
            block_meta.emplace(range.block_id_, segment_meta.value(), table_index_meta.kv_instance());
            column_meta.emplace(column_id, *block_meta, block_meta->kv_instance());
            cur_offset = range.start_offset_;
            cur_row_cnt = range.row_count_;
        } else {
            if (cur_offset + cur_row_cnt != range.start_offset_) {
                UnrecoverableError("AppendIndex: append range is not continuous");
            }
            cur_row_cnt += range.row_count_;
        }
    }
    if (cur_row_cnt > 0) {
        append_in_column();
    }

    return Status::OK();
}

Status
NewTxn::AppendMemIndex(SegmentIndexMeta &segment_index_meta, RowID base_row_id, const ColumnVector &col, BlockOffset offset, BlockOffset row_cnt) {
    SharedPtr<IndexBase> index_def;
    {
        Status status = segment_index_meta.table_index_meta().GetIndexDef(index_def);
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<MemIndex> mem_index;
    {
        Status status = segment_index_meta.GetAndWriteMemIndex(mem_index);
        if (!status.ok()) {
            return status;
        }
    }
    switch (index_def->index_type_) {
        case IndexType::kSecondary: {
            SharedPtr<SecondaryIndexInMem> memory_secondary_index;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_secondary_index_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }
                    mem_index->memory_secondary_index_ = SecondaryIndexInMem::NewSecondaryIndexInMem(column_def, nullptr, base_row_id);
                }
                memory_secondary_index = mem_index->memory_secondary_index_;
            }
            memory_secondary_index->InsertBlockData(base_row_id.segment_offset_, col, offset, row_cnt);
            break;
        }
        case IndexType::kFullText: {
            const auto *index_fulltext = static_cast<const IndexFullText *>(index_def.get());
            SharedPtr<MemoryIndexer> memory_indexer;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_indexer_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }

                    SharedPtr<String> index_dir = segment_index_meta.table_index_meta().GetTableIndexDir();
                    String base_name = fmt::format("ft_{:016x}", base_row_id.ToUint64());
                    String full_path = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *index_dir);
                    mem_index->memory_indexer_ =
                        MakeUnique<MemoryIndexer>(full_path, base_name, base_row_id, index_fulltext->flag_, index_fulltext->analyzer_, nullptr);
                    // table_index_entry_->UpdateFulltextSegmentTs(commit_ts);
                } else {
                    RowID exp_begin_row_id = mem_index->memory_indexer_->GetBaseRowId() + mem_index->memory_indexer_->GetDocCount();
                    assert(base_row_id >= exp_begin_row_id);
                    if (base_row_id > exp_begin_row_id) {
                        LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                             base_row_id.ToUint64(),
                                             exp_begin_row_id.ToUint64(),
                                             base_row_id - exp_begin_row_id));
                        mem_index->memory_indexer_->InsertGap(base_row_id - exp_begin_row_id);
                    }
                }
                if (index_fulltext->IsRealtime()) {
                    UnrecoverableError("Not implemented yet");
                } else {
                    auto col_ptr = MakeShared<ColumnVector>(std::move(col));
                    mem_index->memory_indexer_->Insert(col_ptr, offset, row_cnt, false);
                }
            }
            break;
        }
        case IndexType::kIVF: {
            SharedPtr<IVFIndexInMem> memory_ivf_index;
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                if (mem_index->memory_ivf_index_.get() == nullptr) {
                    auto [column_def, status] = segment_index_meta.table_index_meta().GetColumnDef();
                    if (!status.ok()) {
                        return status;
                    }
                    mem_index->memory_ivf_index_ = IVFIndexInMem::NewIVFIndexInMem(column_def.get(), index_def.get(), base_row_id, nullptr);
                }
                memory_ivf_index = mem_index->memory_ivf_index_;
            }
            memory_ivf_index->InsertBlockData(base_row_id.segment_offset_, col, offset, row_cnt);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    return Status::OK();
}

Status NewTxn::PopulateIndex(const String &db_name,
                             const String &table_name,
                             const String &index_name,
                             TableIndexMeeta &table_index_meta,
                             SegmentMeta &segment_meta) {
    Optional<SegmentIndexMeta> segment_index_meta;
    {
        Status status = this->AddNewSegmentIndex(table_index_meta, segment_meta.segment_id(), segment_index_meta);
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<IndexBase> index_def;
    {
        Status status = table_index_meta.GetIndexDef(index_def);
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<ColumnDef> column_def;
    ColumnID column_id = 0;
    {
        auto [column_def_, status] = table_index_meta.table_meta().GetColumnDefByColumnName(index_def->column_name());
        if (!status.ok()) {
            return status;
        }
        column_def = column_def_;
        column_id = column_def->id();
    }
    Vector<ChunkID> old_chunk_ids;
    {
        Vector<ChunkID> *old_chunk_ids_ptr = nullptr;
        Status status = segment_index_meta->GetChunkIDs(old_chunk_ids_ptr);
        if (!status.ok()) {
            return status;
        }
        old_chunk_ids = *old_chunk_ids_ptr;
    }
    ChunkID new_chunk_id = 0;
    if (index_def->index_type_ == IndexType::kIVF) {
        Status status = this->PopulateIvfIndexInner(index_def, *segment_index_meta, segment_meta, column_def, new_chunk_id);
        if (!status.ok()) {
            return status;
        }
    } else {
        switch (index_def->index_type_) {
            case IndexType::kSecondary: {
                Status status = this->PopulateIndexToMem(*segment_index_meta, segment_meta, column_id);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            case IndexType::kFullText: {
                Status status = this->PopulateFtIndexInner(index_def, *segment_index_meta, segment_meta, column_id);
                if (!status.ok()) {
                    return status;
                }
                break;
            }
            default: {
                UnrecoverableError("Invalid index type");
                return Status::OK();
            }
        }
        {
            Status status = DumpMemIndexInner(*segment_index_meta, new_chunk_id);
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        ChunkIndexMeta chunk_index_meta(new_chunk_id, *segment_index_meta, segment_meta.kv_instance());
        Status status = this->AddChunkWal(db_name, table_name, index_name, chunk_index_meta, old_chunk_ids, false);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewTxn::PopulateIndexToMem(SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, ColumnID column_id) {
    Vector<BlockID> *block_ids_ptr = nullptr;
    {
        Status status = segment_meta.GetBlockIDs(block_ids_ptr);
        if (!status.ok()) {
            return status;
        }
    }
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());
        ColumnMeta column_meta(column_id, block_meta, block_meta.kv_instance());

        SizeT row_cnt = 0;
        {
            Status status = block_meta.GetRowCnt(row_cnt);
            if (!status.ok()) {
                return status;
            }
        }
        ColumnVector col;
        {
            Status status = this->GetColumnVector(column_meta, row_cnt, ColumnVectorTipe::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
        }
        SegmentOffset block_offset = block_meta.block_capacity() * block_meta.block_id();
        RowID begin_row_id(segment_meta.segment_id(), block_offset);
        u32 offset = 0;
        Status status = this->AppendMemIndex(segment_index_meta, begin_row_id, col, offset, row_cnt);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status
NewTxn::PopulateFtIndexInner(SharedPtr<IndexBase> index_def, SegmentIndexMeta &segment_index_meta, SegmentMeta &segment_meta, ColumnID column_id) {
    if (index_def->index_type_ != IndexType::kFullText) {
        UnrecoverableError("Invalid index type");
    }
    const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_def.get());
    RowID base_row_id(segment_index_meta.segment_id(), 0);
    String base_name = fmt::format("ft_{:016x}", base_row_id.ToUint64());
    String full_path;
    {
        SharedPtr<String> index_dir = segment_index_meta.table_index_meta().GetTableIndexDir();
        full_path = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *index_dir);
    }
    SharedPtr<MemIndex> mem_index;
    {
        Status status = segment_index_meta.GetAndWriteMemIndex(mem_index);
        if (!status.ok()) {
            return status;
        }
    }
    mem_index->memory_indexer_ =
        MakeUnique<MemoryIndexer>(full_path, base_name, base_row_id, index_fulltext->flag_, index_fulltext->analyzer_, nullptr);
    MemoryIndexer *memory_indexer = mem_index->memory_indexer_.get();

    Vector<BlockID> *block_ids_ptr = nullptr;
    {
        Status status = segment_meta.GetBlockIDs(block_ids_ptr);
        if (!status.ok()) {
            return status;
        }
    }
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());
        ColumnMeta column_meta(column_id, block_meta, block_meta.kv_instance());

        SizeT row_cnt = 0;
        {
            Status status = block_meta.GetRowCnt(row_cnt);
            if (!status.ok()) {
                return status;
            }
        }
        ColumnVector col;
        {
            Status status = this->GetColumnVector(column_meta, row_cnt, ColumnVectorTipe::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
        }
        SegmentOffset block_offset = block_meta.block_capacity() * block_meta.block_id();
        RowID begin_row_id(segment_meta.segment_id(), block_offset);

        RowID exp_begin_row_id = memory_indexer->GetBaseRowId() + memory_indexer->GetDocCount();
        assert(begin_row_id >= exp_begin_row_id);
        if (begin_row_id > exp_begin_row_id) {
            LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                 begin_row_id.ToUint64(),
                                 exp_begin_row_id.ToUint64(),
                                 begin_row_id - exp_begin_row_id));
            memory_indexer->InsertGap(begin_row_id - exp_begin_row_id);
        }

        auto col_ptr = MakeShared<ColumnVector>(std::move(col));
        memory_indexer->Insert(col_ptr, 0, row_cnt, true);
        memory_indexer->Commit(true);
    }
    return Status::OK();
}

Status NewTxn::PopulateIvfIndexInner(SharedPtr<IndexBase> index_def,
                                     SegmentIndexMeta &segment_index_meta,
                                     SegmentMeta &segment_meta,
                                     SharedPtr<ColumnDef> column_def,
                                     ChunkID &new_chunk_id) {
    RowID base_row_id(segment_index_meta.segment_id(), 0);
    u32 row_count = 0;
    {
        auto [rc, status] = segment_meta.GetRowCnt();
        if (!status.ok()) {
            return status;
        }
        row_count = rc;
    }
    ChunkID chunk_id = 0;
    {
        Status status = segment_index_meta.GetNextChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            return status;
        }
    }
    new_chunk_id = chunk_id;
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        Status status = this->AddNewChunkIndex(segment_index_meta, chunk_id, base_row_id, row_count, "" /*base_name*/, chunk_index_meta, buffer_obj);
        if (!status.ok()) {
            return status;
        }
    }
    {
        BufferHandle buffer_handle = buffer_obj->Load();
        auto *data_ptr = static_cast<IVFIndexInChunk *>(buffer_handle.GetDataMut());
        data_ptr->BuildIVFIndex(this, segment_meta, row_count, column_def);
    }
    buffer_obj->Save();
    return Status::OK();
}

Status NewTxn::OptimizeFtIndex(SharedPtr<IndexBase> index_def,
                               SegmentIndexMeta &segment_index_meta,
                               RowID &base_rowid_out,
                               u32 &row_cnt_out,
                               String &base_name_out) {
    const auto *index_fulltext = static_cast<const IndexFullText *>(index_def.get());

    Vector<ChunkID> chunk_ids;
    {
        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        Status status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
        if (!status.ok()) {
            return status;
        }
        chunk_ids = *chunk_ids_ptr;
    }
    if (chunk_ids.size() <= 1) {
        return Status::OK();
    }
    String msg = fmt::format("merging {}", *index_fulltext->index_name_);
    Vector<String> base_names;
    Vector<RowID> base_rowids;
    RowID base_rowid;
    u32 total_row_count = 0;
    Vector<ChunkID> old_ids;

    for (auto iter = chunk_ids.begin(); iter != chunk_ids.end(); ++iter) {
        ChunkID chunk_id = *iter;
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, segment_index_meta.kv_instance());
        ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
        {
            Status status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
            if (!status.ok()) {
                return status;
            }
        }
        if (chunk_id == 0) {
            base_rowid = chunk_info_ptr->base_row_id_;
        }
        if (const RowID expect_base_row_id = base_rowid + total_row_count; chunk_info_ptr->base_row_id_ > expect_base_row_id) {
            msg += fmt::format(" stop at gap to chunk {}, expect_base_row_id: {:016x}, base_row_id: {:016x}",
                               chunk_info_ptr->base_name_,
                               expect_base_row_id.ToUint64(),
                               chunk_info_ptr->base_row_id_.ToUint64());
            chunk_ids.erase(iter, chunk_ids.end());
            break;
        } else if (chunk_info_ptr->base_row_id_ < expect_base_row_id) {
            msg += fmt::format(" found overlap to chunk {}, expect_base_row_id: {:016x}, base_row_id: {:016x}",
                               chunk_info_ptr->base_name_,
                               expect_base_row_id.ToUint64(),
                               chunk_info_ptr->base_row_id_.ToUint64());
            UnrecoverableError(msg);
            return Status::OK();
        }
        msg += " " + chunk_info_ptr->base_name_;
        base_names.push_back(chunk_info_ptr->base_name_);
        base_rowids.push_back(chunk_info_ptr->base_row_id_);
        total_row_count += chunk_info_ptr->row_cnt_;
    }

    if (chunk_ids.size() <= 1) {
        msg += fmt::format(" skip merge due to only {} chunk", chunk_ids.size());
        LOG_INFO(msg);
        return Status::OK();
    }

    String dst_base_name = fmt::format("ft_{:016x}_{:x}", base_rowid.ToUint64(), total_row_count);
    msg += " -> " + dst_base_name;
    LOG_INFO(msg);

    SharedPtr<String> index_dir = segment_index_meta.table_index_meta().GetTableIndexDir();
    ColumnIndexMerger column_index_merger(*index_dir, index_fulltext->flag_);
    column_index_merger.Merge(base_names, base_rowids, dst_base_name);
    {
        base_rowid_out = base_rowid;
        row_cnt_out = total_row_count;
        base_name_out = dst_base_name;
    }

    // // OPTIMIZE invoke this func at which the txn hasn't been commited yet.
    // TxnTimeStamp ts = std::max(txn->BeginTS(), txn->CommitTS());
    // table_index_entry->UpdateFulltextSegmentTs(ts);
    LOG_INFO(fmt::format("done merging {} {}", *index_fulltext->index_name_, dst_base_name));

    return Status::OK();
}

Status NewTxn::DumpMemIndexInner(SegmentIndexMeta &segment_index_meta, ChunkID &new_chunk_id) {
    TableIndexMeeta &table_index_meta = segment_index_meta.table_index_meta();
    SharedPtr<IndexBase> index_base;
    {
        Status status = table_index_meta.GetIndexDef(index_base);
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<MemIndex> mem_index;
    {
        Status status = segment_index_meta.GetMemIndex(mem_index);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.SetNoMemIndex();
        if (!status.ok()) {
            return status;
        }
    }
    ChunkID chunk_id = 0;
    {
        Status status = segment_index_meta.GetNextChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            return status;
        }
    }
    new_chunk_id = chunk_id;
    RowID row_id{};
    u32 row_count = 0;
    String base_name;

    // dump mem index only happens in parallel with read, not write, so no lock is needed.
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            row_id = mem_index->memory_secondary_index_->GetBeginRowID();
            row_count = mem_index->memory_secondary_index_->GetRowCount();
            break;
        }
        case IndexType::kFullText: {
            base_name = mem_index->memory_indexer_->GetBaseName();
            row_id = mem_index->memory_indexer_->GetBaseRowId();
            row_count = mem_index->memory_indexer_->GetDocCount();
            break;
        }
        case IndexType::kIVF: {
            row_id = mem_index->memory_ivf_index_->GetBeginRowID();
            row_count = mem_index->memory_ivf_index_->GetRowCount();
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        Status status = this->AddNewChunkIndex(segment_index_meta, chunk_id, row_id, row_count, base_name, chunk_index_meta, buffer_obj);
        if (!status.ok()) {
            return status;
        }
    }
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            mem_index->memory_secondary_index_->Dump(buffer_obj);
            buffer_obj->Save();
            break;
        }
        case IndexType::kFullText: {
            mem_index->memory_indexer_->Dump(false /*offline*/, false /*spill*/);
            u64 len_sum = mem_index->memory_indexer_->GetColumnLengthSum();
            u32 len_cnt = mem_index->memory_indexer_->GetDocCount();
            Status status = segment_index_meta.UpdateFtInfo(len_sum, len_cnt);
            if (!status.ok()) {
                return status;
            }
            break;
        }
        case IndexType::kIVF: {
            mem_index->memory_ivf_index_->Dump(buffer_obj);
            buffer_obj->Save();
            break;
        }
        // case IndexType::kHnsw:
        // case IndexType::kBMP: {
        //     buffer_obj->ToMmap();
        //     break;
        // }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    return Status::OK();
}

Status NewTxn::AddChunkWal(const String &db_name,
                           const String &table_name,
                           const String &index_name,
                           ChunkIndexMeta &chunk_index_meta,
                           const Vector<ChunkID> &deprecate_ids,
                           bool clear_mem_index) {
    SegmentIndexMeta &segment_index_meta = chunk_index_meta.segment_index_meta();
    Vector<WalChunkIndexInfo> chunk_infos;
    chunk_infos.emplace_back(chunk_index_meta);
    SegmentID segment_id = segment_index_meta.segment_id();
    auto dump_cmd = MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id, std::move(chunk_infos), deprecate_ids);
    if (clear_mem_index) {
        dump_cmd->clear_mem_index_ = true;
        dump_cmd->db_id_str_ = segment_index_meta.table_index_meta().table_meta().db_id_str();
        dump_cmd->table_id_str_ = segment_index_meta.table_index_meta().table_meta().table_id_str();
        dump_cmd->index_id_str_ = segment_index_meta.table_index_meta().index_id_str();
    }

    wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(dump_cmd));
    txn_context_ptr_->AddOperation(MakeShared<String>(dump_cmd->ToString()));

    return Status::OK();
}

Status NewTxn::GetChunkIndex(ChunkIndexMeta &chunk_index_meta, BufferObj *&buffer_obj) {
    TableIndexMeeta &table_index_meta = chunk_index_meta.segment_index_meta().table_index_meta();

    String index_dir = fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), table_index_meta.GetTableIndexDir()->c_str());
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    SharedPtr<IndexBase> index_base;
    {
        Status status = table_index_meta.GetIndexDef(index_base);
        if (!status.ok()) {
            return status;
        }
    }
    SegmentID segment_id = chunk_index_meta.segment_index_meta().segment_id();
    ChunkID chunk_id = chunk_index_meta.chunk_id();
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            String secondary_index_file_name = IndexFileName(segment_id, chunk_id);
            String index_filepath = fmt::format("{}/{}", index_dir, secondary_index_file_name);
            buffer_obj = buffer_mgr->GetBufferObject(index_filepath);
            if (buffer_obj == nullptr) {
                return Status::BufferManagerError("GetBufferObject failed");
            }
            break;
        }
        case IndexType::kFullText: {
            ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
            {
                Status status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
                if (!status.ok()) {
                    return status;
                }
            }
            auto column_length_file_name = chunk_info_ptr->base_name_ + LENGTH_SUFFIX;
            String index_filepath = fmt::format("{}/{}", index_dir, column_length_file_name);
            buffer_obj = buffer_mgr->GetBufferObject(index_filepath);
            if (buffer_obj == nullptr) {
                return Status::BufferManagerError("GetBufferObject failed");
            }
            break;
        }
        case IndexType::kIVF: {
            String ivf_index_file_name = IndexFileName(segment_id, chunk_id);
            String index_filepath = fmt::format("{}/{}", index_dir, ivf_index_file_name);
            buffer_obj = buffer_mgr->GetBufferObject(index_filepath);
            if (buffer_obj == nullptr) {
                return Status::BufferManagerError("GetBufferObject failed");
            }
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }

    return Status::OK();
}

Status NewTxn::CommitCreateIndex(const WalCmdCreateIndex *create_index_cmd) {
    TxnTimeStamp commit_ts = txn_context_ptr_->commit_ts_;

    const String &db_name = create_index_cmd->db_name_;
    const String &table_name = create_index_cmd->table_name_;
    const String &index_name = *create_index_cmd->index_base_->index_name_;
    const String &db_id_str = create_index_cmd->db_id_;
    const String &table_id_str = create_index_cmd->table_id_;

    // Get latest index id and lock the id
    String index_id_str;
    Status status = IncrLatestID(index_id_str, LATEST_INDEX_ID);
    if (!status.ok()) {
        return status;
    }

    // Create index key value pair
    String index_key = KeyEncode::CatalogIndexKey(db_id_str, table_id_str, index_name, commit_ts);
    status = kv_instance_->Put(index_key, index_id_str);
    if (!status.ok()) {
        return status;
    }

    TableMeeta table_meta(db_id_str, table_id_str, *kv_instance_);
    TableIndexMeeta table_index_meta(index_id_str, table_meta, *kv_instance_);
    {
        Status status = table_index_meta.InitSet(create_index_cmd->index_base_);
        if (!status.ok()) {
            return status;
        }
    }
    {
        auto [segment_ids_ptr, status] = table_meta.GetSegmentIDs();
        if (!status.ok()) {
            return status;
        }
        for (SegmentID segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, table_meta, table_meta.kv_instance());
            Status status = this->PopulateIndex(db_name, table_name, index_name, table_index_meta, segment_meta);
            if (!status.ok()) {
                return status;
            }
        }
    }
    if (create_index_cmd->index_base_->index_type_ == IndexType::kFullText) {
        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        String ft_cache_key = table_index_meta.FtIndexCacheTag();
        auto ft_cache = MakeShared<TableIndexReaderCache>(db_id_str, table_id_str);
        Status status = new_catalog->AddFtIndexCache(std::move(ft_cache_key), std::move(ft_cache));
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::PostCommitDumpIndex(const WalCmdDumpIndex *dump_index_cmd) {
    KVStore *kv_store = txn_mgr_->kv_store();
    UniquePtr<KVInstance> kv_instance = kv_store->GetInstance();

    if (dump_index_cmd->clear_mem_index_) {
        const String &db_id_str_ = dump_index_cmd->db_id_str_;
        const String &table_id_str_ = dump_index_cmd->table_id_str_;
        TableMeeta table_meta(db_id_str_, table_id_str_, *kv_instance);

        const String &index_id_str_ = dump_index_cmd->index_id_str_;
        TableIndexMeeta table_index_meta(index_id_str_, table_meta, table_meta.kv_instance());

        SegmentIndexMeta segment_index_meta(dump_index_cmd->segment_id_, table_index_meta, table_index_meta.kv_instance());
        SharedPtr<MemIndex> mem_index;
        Status status = segment_index_meta.GetMemIndex(mem_index);
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

} // namespace infinity

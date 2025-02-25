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

#include <vector>

module new_txn;

import chunk_index_meta;
import segment_index_meta;
import table_index_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;

import infinity_context;
import buffer_manager;
import third_party;
import infinity_exception;
import secondary_index_file_worker;
import column_def;
import internal_types;
import index_file_worker;
import create_index_info;
import buffer_obj;
import mem_index;
import wal_entry;

import secondary_index_in_mem;
import secondary_index_data;

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

    TableMeeta table_meta(db_id_str, table_id_str, db_name, table_name, *kv_instance_);
    TableIndexMeeta table_index_meta(index_id_str, index_name, table_meta, table_meta.kv_instance());
    SegmentIndexMeta segment_index_meta(segment_id, table_index_meta, table_index_meta.kv_instance());

    return this->DumpMemIndexInner(segment_index_meta);
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

    TableMeeta table_meta(db_id_str, table_id_str, db_name, table_name, *kv_instance_);
    TableIndexMeeta table_index_meta(index_id_str, index_name, table_meta, table_meta.kv_instance());
    SegmentIndexMeta segment_index_meta(segment_id, table_index_meta, table_index_meta.kv_instance());

    Vector<ChunkIndexMeta> old_chunk_metas;
    {
        Vector<ChunkID> *chunk_ids;
        {
            Status status = segment_index_meta.GetChunkIDs(chunk_ids);
            if (!status.ok()) {
                return status;
            }
        }
        for (ChunkID chunk_id : *chunk_ids) {
            old_chunk_metas.emplace_back(chunk_id, segment_index_meta, segment_index_meta.kv_instance());
        }
    }
    RowID base_rowid(segment_id, 0);
    u32 row_cnt = 0;
    Vector<Pair<u32, BufferObj *>> old_buffers;
    {
        RowID last_rowid = base_rowid;
        ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
        for (ChunkIndexMeta &old_chunk_meta : old_chunk_metas) {
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
    SharedPtr<IndexBase> index_base;
    {
        Status status = table_index_meta.GetIndexDef(index_base);
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
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            Status status = this->AddNewChunkIndex(segment_index_meta, chunk_id, base_rowid, row_cnt, chunk_index_meta, buffer_obj);
            if (!status.ok()) {
                return status;
            }
            BufferHandle buffer_handle = buffer_obj->Load();
            auto *data_ptr = static_cast<SecondaryIndexData *>(buffer_handle.GetDataMut());
            data_ptr->InsertMergeData(old_buffers);
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
        Vector<ChunkID> deprecate_ids;
        for (const ChunkIndexMeta &old_chunk_meta : old_chunk_metas) {
            deprecate_ids.push_back(old_chunk_meta.chunk_id());
        }
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
        Status status = table_index_meta.GetColumnDef(column_def);
        if (!status.ok()) {
            return status;
        }
    }

    SharedPtr<String> index_dir;
    {
        String *table_dir_ptr = nullptr;
        {
            Status status = table_index_meta.table_meta().GetTableDir(table_dir_ptr);
            if (!status.ok()) {
                return status;
            }
        }
        String *index_dir_ptr = nullptr;
        {
            Status status = table_index_meta.GetIndexDir(index_dir_ptr);
            if (!status.ok()) {
                return status;
            }
        }
        index_dir = MakeShared<String>(fmt::format("{}/{}", *table_dir_ptr, *index_dir_ptr));
    }
    ChunkIndexMetaInfo chunk_info;
    {
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
        UniquePtr<IndexFileWorker> index_file_worker;
        switch (index_base->index_type_) {
            case IndexType::kSecondary: {
                auto secondary_index_file_name = MakeShared<String>(IndexFileName(segment_id, chunk_id));
                index_file_worker = MakeUnique<SecondaryIndexFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                         MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                         index_dir,
                                                                         std::move(secondary_index_file_name),
                                                                         index_base,
                                                                         column_def,
                                                                         row_count,
                                                                         buffer_mgr->persistence_manager());
                break;
            }
            default: {
                UnrecoverableError("Not implemented yet");
            }
        }
        buffer_obj = buffer_mgr->AllocateBufferObject(std::move(index_file_worker));
        if (buffer_obj == nullptr) {
            return Status::BufferManagerError("AllocateBufferObject failed");
        }
        buffer_obj->AddObjRc();
    }
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
        SharedPtr<ColumnDef> column_def;
        Status status = table_index_meta.table_meta().GetColumnDefByColumnName(index_def->column_name(), column_def);
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
        Status status = segment_index_meta.GetMemIndex(mem_index);
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
                    SharedPtr<ColumnDef> column_def;
                    {
                        Status status = segment_index_meta.table_index_meta().GetColumnDef(column_def);
                        if (!status.ok()) {
                            return status;
                        }
                    }
                    mem_index->memory_secondary_index_ = SecondaryIndexInMem::NewSecondaryIndexInMem(column_def, nullptr, base_row_id);
                }
                memory_secondary_index = mem_index->memory_secondary_index_;
            }
            memory_secondary_index->InsertBlockData(base_row_id.segment_offset_, col, offset, row_cnt);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    return Status::OK();
}

Status NewTxn::PopulateIndex(TableIndexMeeta &table_index_meta, SegmentMeta &segment_meta) {
    Optional<SegmentIndexMeta> segment_index_meta;
    {
        Status status = this->AddNewSegmentIndex(table_index_meta, segment_meta.segment_id(), segment_index_meta);
        if (status.ok()) {
            return status;
        }
    }
    SharedPtr<MemIndex> mem_index;
    {
        Status status = segment_index_meta->GetMemIndex(mem_index);
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
    ColumnID column_id = 0;
    {
        SharedPtr<ColumnDef> column_def;
        Status status = table_index_meta.table_meta().GetColumnDefByColumnName(index_def->column_name(), column_def);
        if (!status.ok()) {
            return status;
        }
        column_id = column_def->id();
    }
    Vector<BlockID> *block_ids = nullptr;
    {
        Status status = segment_meta.GetBlockIDs(block_ids);
        if (!status.ok()) {
            return status;
        }
    }
    for (BlockID block_id : *block_ids) {
        BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());
        SizeT cur_row_cnt = 0;
        {
            Status status = block_meta.GetRowCnt(cur_row_cnt);
            if (!status.ok()) {
                return status;
            }
        }
        ColumnMeta column_meta(column_id, block_meta, block_meta.kv_instance());

        SegmentOffset block_offset = block_meta.block_capacity() * block_meta.block_id() + cur_row_cnt; // TODO
        RowID base_rowid(segment_meta.segment_id(), block_offset);

        SizeT row_cnt = 0;
        u32 offset = 0;
        ColumnVector col;
        {
            Status status = block_meta.GetRowCnt(row_cnt);
            if (!status.ok()) {
                return status;
            }
        }
        {
            Status status = this->GetColumnVector(column_meta, row_cnt, ColumnVectorTipe::kReadOnly, col);
            if (!status.ok()) {
                return status;
            }
        }
        {
            Status status = this->AppendMemIndex(*segment_index_meta, base_rowid, col, offset, row_cnt);
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        Status status = DumpMemIndexInner(*segment_index_meta);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewTxn::DumpMemIndexInner(SegmentIndexMeta &segment_index_meta) {
    TableIndexMeeta &table_index_meta = segment_index_meta.table_index_meta();
    SegmentID segment_id = segment_index_meta.segment_id();
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
    RowID row_id{};
    u32 row_count = 0;

    MemIndex mem_index_copy;
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            {
                std::unique_lock<std::mutex> lock(mem_index->mtx_);
                mem_index_copy.memory_secondary_index_ = std::exchange(mem_index->memory_secondary_index_, nullptr);
            }
            if (mem_index_copy.memory_secondary_index_.get() == nullptr) {
                return Status::OK();
            }
            row_id = mem_index_copy.memory_secondary_index_->GetBeginRowID();
            row_count = mem_index_copy.memory_secondary_index_->GetRowCount();
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    Optional<ChunkIndexMeta> chunk_index_meta;
    BufferObj *buffer_obj = nullptr;
    {
        Status status = this->AddNewChunkIndex(segment_index_meta, chunk_id, row_id, row_count, chunk_index_meta, buffer_obj);
        if (!status.ok()) {
            return status;
        }
    }
    switch (index_base->index_type_) {
        case IndexType::kSecondary: {
            mem_index_copy.memory_secondary_index_->Dump(buffer_obj);
            break;
        }
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }
    {
        const String &db_name = table_index_meta.table_meta().db_name();
        const String &table_name = table_index_meta.table_meta().table_name();
        const String &index_name = table_index_meta.index_name();

        Vector<WalChunkIndexInfo> chunk_infos;
        chunk_infos.emplace_back(*chunk_index_meta);
        Vector<ChunkID> deprecate_ids;
        auto dump_cmd = MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id, std::move(chunk_infos), std::move(deprecate_ids));

        wal_entry_->cmds_.push_back(static_pointer_cast<WalCmd>(dump_cmd));
        txn_context_ptr_->AddOperation(MakeShared<String>(dump_cmd->ToString()));
    }
    return Status::OK();
}

Status NewTxn::GetChunkIndex(ChunkIndexMeta &chunk_index_meta, BufferObj *&buffer_obj) {
    TableIndexMeeta &table_index_meta = chunk_index_meta.segment_index_meta().table_index_meta();
    TableMeeta &table_meta = table_index_meta.table_meta();

    String index_dir;
    {
        String *table_dir_ptr = nullptr;
        {
            Status status = table_meta.GetTableDir(table_dir_ptr);
            if (!status.ok()) {
                return status;
            }
        }
        String *index_dir_ptr = nullptr;
        {
            Status status = table_index_meta.GetIndexDir(index_dir_ptr);
            if (!status.ok()) {
                return status;
            }
        }
        index_dir = fmt::format("{}/{}/{}", InfinityContext::instance().config()->DataDir(), *table_dir_ptr, *index_dir_ptr);
    }
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
        default: {
            UnrecoverableError("Not implemented yet");
        }
    }

    return Status::OK();
}

} // namespace infinity

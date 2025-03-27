// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module new_catalog;

import stl;
import third_party;
import block_version;
import infinity_exception;
import table_def;
import kv_code;
import kv_store;
import column_vector;
import wal_entry;
import infinity_context;
import logger;
import default_values;
import new_txn;
import data_access_state;

import catalog_meta;
import db_meeta;
import table_meeta;
import segment_meta;
import block_meta;
import column_meta;
import table_index_meeta;
import segment_index_meta;
import chunk_index_meta;

namespace infinity {

// namespace {

// String IndexFileName(SegmentID segment_id, ChunkID chunk_id) { return fmt::format("seg{}_chunk{}.idx", segment_id, chunk_id); }

// } // namespace

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
    const auto *block_version = reinterpret_cast<const BlockVersion *>(version_buffer_handle_.GetData());

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

Status NewCatalog::InitCatalog(KVInstance *kv_instance, TxnTimeStamp checkpoint_ts) {
    Status status;
    TxnTimeStamp begin_ts = checkpoint_ts;

    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(*kv_instance);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }

    auto InitBlockColumn = [&](ColumnMeta &column_meta) {
        Status status = column_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }
        return Status::OK();
    };
    auto InitBlock = [&](BlockMeta &block_meta) {
        SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
        std::tie(column_defs_ptr, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
        for (const auto &column_def : *column_defs_ptr) {
            ColumnMeta column_meta(column_def->id(), block_meta, *kv_instance);
            status = InitBlockColumn(column_meta);
            if (!status.ok()) {
                return status;
            }
        }
        Status status = block_meta.LoadSet(checkpoint_ts);
        if (!status.ok()) {
            return status;
        }

        return Status::OK();
    };
    auto InitSegment = [&](SegmentMeta &segment_meta) {
        auto [block_ids, status] = segment_meta.GetBlockIDs1(begin_ts);
        if (!status.ok()) {
            return status;
        }
        for (BlockID block_id : *block_ids) {
            BlockMeta block_meta(block_id, segment_meta, *kv_instance);
            status = InitBlock(block_meta);
            if (!status.ok()) {
                return status;
            }
        }

        return Status::OK();
    };
    auto InitChunkIndex = [&](ChunkID chunk_id, SegmentIndexMeta &segment_index_meta) {
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, *kv_instance);
        Status status = chunk_index_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }
        return Status::OK();
    };
    auto InitSegmentIndex = [&](SegmentID segment_id, TableIndexMeeta &table_index_meta) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta, *kv_instance);
        status = segment_index_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }

        Vector<ChunkID> *chunk_ids_ptr = nullptr;
        status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
        if (!status.ok()) {
            return status;
        }
        for (ChunkID chunk_id : *chunk_ids_ptr) {
            status = InitChunkIndex(chunk_id, segment_index_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    auto InitTableIndex = [&](const String &index_id_str, TableMeeta &table_meta) {
        TableIndexMeeta table_index_meta(index_id_str, table_meta, *kv_instance);

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        status = table_index_meta.GetSegmentIDs(segment_ids_ptr);
        if (!status.ok()) {
            return status;
        }
        for (SegmentID segment_id : *segment_ids_ptr) {
            status = InitSegmentIndex(segment_id, table_index_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    auto InitTable = [&](const String &table_id_str, DBMeeta &db_meta) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, *kv_instance);

        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1(begin_ts);
        if (!status.ok()) {
            return status;
        }
        for (SegmentID segment_id : *segment_ids_ptr) {
            SegmentMeta segment_meta(segment_id, table_meta, *kv_instance);
            status = InitSegment(segment_meta);
            if (!status.ok()) {
                return status;
            }
        }

        Vector<String> *index_id_strs_ptr = nullptr;
        status = table_meta.GetIndexIDs(index_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &index_id_str : *index_id_strs_ptr) {
            TableIndexMeeta table_index_meta(index_id_str, table_meta, *kv_instance);
            status = InitTableIndex(index_id_str, table_meta);
            if (!status.ok()) {
                return status;
            }
        }

        return Status::OK();
    };
    auto InitDB = [&](const String &db_id_str) {
        DBMeeta db_meta(db_id_str, *kv_instance);

        Vector<String> *table_id_strs_ptr = nullptr;
        status = db_meta.GetTableIDs(table_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &table_id_str : *table_id_strs_ptr) {
            status = InitTable(table_id_str, db_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    for (const String &db_id_str : *db_id_strs_ptr) {
        status = InitDB(db_id_str);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewCatalog::MemIndexRecover(NewTxn *txn, TxnTimeStamp system_start_ts) {
    Status status;
    KVInstance *kv_instance = txn->kv_instance();

    Vector<String> *db_id_strs_ptr;
    CatalogMeta catalog_meta(*kv_instance);
    status = catalog_meta.GetDBIDs(db_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    auto IndexRecoverTable = [&](TableMeeta &table_meta) {
        Vector<String> *index_id_strs_ptr = nullptr;
        status = table_meta.GetIndexIDs(index_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &index_id_str : *index_id_strs_ptr) {
            TableIndexMeeta table_index_meta(index_id_str, table_meta, *kv_instance);
            status = txn->RecoverMemIndex(table_index_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    auto IndexRecoverDB = [&](DBMeeta &db_meta) {
        Vector<String> *table_id_strs_ptr = nullptr;
        status = db_meta.GetTableIDs(table_id_strs_ptr);
        if (!status.ok()) {
            return status;
        }
        for (const String &table_id_str : *table_id_strs_ptr) {
            TableMeeta table_meta(db_meta.db_id_str(), table_id_str, *kv_instance);
            status = IndexRecoverTable(table_meta);
            if (!status.ok()) {
                return status;
            }
        }
        return Status::OK();
    };
    for (const String &db_id_str : *db_id_strs_ptr) {
        DBMeeta db_meta(db_id_str, *kv_instance);
        status = IndexRecoverDB(db_meta);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::AddNewDB(KVInstance *kv_instance,
                            const String &db_id_str,
                            TxnTimeStamp commit_ts,
                            const String &db_name,
                            const String *db_comment,
                            Optional<DBMeeta> &db_meta) {
    String db_key = KeyEncode::CatalogDbKey(db_name, commit_ts);
    Status status = kv_instance->Put(db_key, db_id_str);
    if (!status.ok()) {
        return status;
    }

    db_meta.emplace(db_id_str, *kv_instance);
    status = db_meta->InitSet(db_comment);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::CleanDB(DBMeeta &db_meta, TxnTimeStamp begin_ts) {
    Status status;

    Vector<String> *table_id_strs_ptr = nullptr;
    status = db_meta.GetTableIDs(table_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }

    for (const String &table_id_str : *table_id_strs_ptr) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, db_meta.kv_instance());
        status = NewCatalog::CleanTable(table_meta, begin_ts);
        if (!status.ok()) {
            return status;
        }
    }

    status = db_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewTable(DBMeeta &db_meta,
                               const String &table_id_str,
                               TxnTimeStamp commit_ts,
                               const SharedPtr<TableDef> &table_def,
                               Optional<TableMeeta> &table_meta) {
    // Create table key value pair
    KVInstance &kv_instance = db_meta.kv_instance();
    String table_key = KeyEncode::CatalogTableKey(db_meta.db_id_str(), *table_def->table_name(), commit_ts);
    Status status = kv_instance.Put(table_key, table_id_str);
    if (!status.ok()) {
        return status;
    }

    table_meta.emplace(db_meta.db_id_str(), table_id_str, db_meta.kv_instance());
    status = table_meta->InitSet(table_def);
    if (!status.ok()) {
        return status;
    }

    return status;
}

Status NewCatalog::CleanTable(TableMeeta &table_meta, TxnTimeStamp begin_ts) {
    Status status;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1(begin_ts);
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta, table_meta.kv_instance());
        status = NewCatalog::CleanSegment(segment_meta, begin_ts);
        if (!status.ok()) {
            return status;
        }
    }

    Vector<String> *index_id_strs_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &index_id_str : *index_id_strs_ptr) {
        TableIndexMeeta table_index_meta(index_id_str, table_meta, table_meta.kv_instance());
        status = NewCatalog::CleanTableIndex(table_index_meta);
        if (!status.ok()) {
            return status;
        }
    }

    status = table_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewTableIndex(TableMeeta &table_meta,
                                    const String &index_id_str,
                                    TxnTimeStamp commit_ts,
                                    const SharedPtr<IndexBase> &index_base,
                                    Optional<TableIndexMeeta> &table_index_meta) {
    // Create index key value pair
    KVInstance &kv_instance = table_meta.kv_instance();
    const String &index_name = *index_base->index_name_;
    String index_key = KeyEncode::CatalogIndexKey(table_meta.db_id_str(), table_meta.table_id_str(), index_name, commit_ts);
    Status status = kv_instance.Put(index_key, index_id_str);
    if (!status.ok()) {
        return status;
    }

    table_index_meta.emplace(index_id_str, table_meta, table_meta.kv_instance());
    status = table_index_meta->InitSet(index_base);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewCatalog::CleanTableIndex(TableIndexMeeta &table_index_meta) {
    Status status;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    status = table_index_meta.GetSegmentIDs(segment_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta, table_index_meta.kv_instance());
        status = NewCatalog::CleanSegmentIndex(segment_index_meta);
        if (!status.ok()) {
            return status;
        }
    }

    status = table_index_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewSegment(TableMeeta &table_meta, SegmentID segment_id, Optional<SegmentMeta> &segment_meta) {
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

Status NewCatalog::AddNewSegment1(TableMeeta &table_meta, TxnTimeStamp commit_ts, Optional<SegmentMeta> &segment_meta) {
    Status status;
    SegmentID segment_id = 0;
    std::tie(segment_id, status) = table_meta.AddSegmentID1(commit_ts);
    if (!status.ok()) {
        return status;
    }
    segment_meta.emplace(segment_id, table_meta, table_meta.kv_instance());
    status = segment_meta->InitSet();
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewCatalog::LoadFlushedSegment1(TableMeeta &table_meta, const WalSegmentInfo &segment_info, TxnTimeStamp checkpoint_ts) {
    Status status;

    SegmentID segment_id = 0;
    std::tie(segment_id, status) = table_meta.AddSegmentID1(checkpoint_ts);
    if (!status.ok()) {
        return status;
    }
    if (segment_id != segment_info.segment_id_) {
        UnrecoverableError(fmt::format("Segment id mismatch, expect: {}, actual: {}", segment_id, segment_info.segment_id_));
    }

    SegmentMeta segment_meta(segment_id, table_meta, table_meta.kv_instance());
    status = segment_meta.InitSet();
    if (!status.ok()) {
        return status;
    }
    for (const WalBlockInfo &block_info : segment_info.block_infos_) {
        status = NewCatalog::LoadFlushedBlock1(segment_meta, block_info, checkpoint_ts);
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status NewCatalog::CleanSegment(SegmentMeta &segment_meta, TxnTimeStamp begin_ts) {
    auto [block_ids, status] = segment_meta.GetBlockIDs1(begin_ts);
    if (!status.ok()) {
        return status;
    }
    for (BlockID block_id : *block_ids) {
        BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());
        status = NewCatalog::CleanBlock(block_meta);
        if (!status.ok()) {
            return status;
        }
    }
    segment_meta.UninitSet();

    return Status::OK();
}

Status NewCatalog::AddNewBlock(SegmentMeta &segment_meta, BlockID block_id, Optional<BlockMeta> &block_meta) {
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
        Status status = NewCatalog::AddNewBlockColumn(*block_meta, column_idx, column_meta);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::AddNewBlock1(SegmentMeta &segment_meta, TxnTimeStamp commit_ts, Optional<BlockMeta> &block_meta) {
    Status status;

    BlockID block_id;
    std::tie(block_id, status) = segment_meta.AddBlockID1(commit_ts);
    if (!status.ok()) {
        return status;
    }
    block_meta.emplace(block_id, segment_meta, segment_meta.kv_instance());
    status = block_meta->InitSet();
    if (!status.ok()) {
        return status;
    }
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    {
        TableMeeta &table_meta = segment_meta.table_meta();
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
    }
    for (const auto &column_def : *column_defs_ptr) {
        ColumnMeta column_meta(column_def->id(), *block_meta, block_meta->kv_instance());
        status = column_meta.InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::LoadFlushedBlock1(SegmentMeta &segment_meta, const WalBlockInfo &block_info, TxnTimeStamp checkpoint_ts) {
    Status status;

    auto *pm = InfinityContext::instance().persistence_manager();
    if (pm) {
        block_info.addr_serializer_.AddToPersistenceManager(pm);
    }

    BlockID block_id = 0;
    std::tie(block_id, status) = segment_meta.AddBlockID1(checkpoint_ts);
    if (!status.ok()) {
        return status;
    }
    if (block_id != block_info.block_id_) {
        UnrecoverableError(fmt::format("Block id mismatch, expect: {}, actual: {}", block_info.block_id_, block_id));
    }

    BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());
    status = block_meta.LoadSet(checkpoint_ts);
    if (!status.ok()) {
        return status;
    }

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    {
        TableMeeta &table_meta = segment_meta.table_meta();
        std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
        if (!status.ok()) {
            return status;
        }
    }
    for (const auto &column_def : *column_defs_ptr) {
        const auto &[chunk_idx, chunk_offset] = block_info.outline_infos_[column_def->id()];
        ColumnMeta column_meta(column_def->id(), block_meta, block_meta.kv_instance());
        status = column_meta.SetChunkOffset(chunk_offset);
        if (!status.ok()) {
            return status;
        }

        status = column_meta.LoadSet();
        if (!status.ok()) {
            return status;
        }
    }
    // status = block_meta.SetRowCnt(block_info.row_count_);
    // if (!status.ok()) {
    //     return status;
    // }

    return Status::OK();
}

Status NewCatalog::CleanBlock(BlockMeta &block_meta) {
    Status status;
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;

    TableMeeta &table_meta = block_meta.segment_meta().table_meta();
    std::tie(column_defs_ptr, status) = table_meta.GetColumnDefs();
    if (!status.ok()) {
        return status;
    }

    for (const auto &column_def : *column_defs_ptr) {
        ColumnMeta column_meta(column_def->id(), block_meta, block_meta.kv_instance());
        Status status = NewCatalog::CleanBlockColumn(column_meta, column_def.get());
        if (!status.ok()) {
            return status;
        }
    }
    block_meta.UninitSet();

    return Status::OK();
}

Status NewCatalog::AddNewBlockColumn(BlockMeta &block_meta, SizeT column_idx, Optional<ColumnMeta> &column_meta) {
    column_meta.emplace(column_idx, block_meta, block_meta.kv_instance());
    {
        Status status = column_meta->InitSet();
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::CleanBlockColumn(ColumnMeta &column_meta, const ColumnDef *column_def) {
    Status status;

    status = column_meta.UninitSet(column_def);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewSegmentIndex(TableIndexMeeta &table_index_meta, SegmentID segment_id, Optional<SegmentIndexMeta> &segment_index_meta) {
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

Status NewCatalog::CleanSegmentIndex(SegmentIndexMeta &segment_index_meta) {
    Status status;

    Vector<ChunkID> *chunk_ids_ptr = nullptr;
    status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, segment_index_meta.kv_instance());
        status = NewCatalog::CleanChunkIndex(chunk_index_meta);
        if (!status.ok()) {
            return status;
        }
    }
    status = segment_index_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::AddNewChunkIndex(SegmentIndexMeta &segment_index_meta,
                                    ChunkID chunk_id,
                                    RowID base_row_id,
                                    SizeT row_count,
                                    const String &base_name,
                                    SizeT index_size,
                                    Optional<ChunkIndexMeta> &chunk_index_meta) {
    ChunkIndexMetaInfo chunk_info;
    chunk_info.base_name_ = base_name;
    chunk_info.base_row_id_ = base_row_id;
    chunk_info.row_cnt_ = row_count;
    chunk_info.index_size_ = index_size;
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

Status NewCatalog::LoadFlushedChunkIndex(SegmentIndexMeta &segment_index_meta, const WalChunkIndexInfo &chunk_info) {
    Status status;

    auto *pm = InfinityContext::instance().persistence_manager();
    if (pm) {
        chunk_info.addr_serializer_.AddToPersistenceManager(pm);
    }

    ChunkID chunk_id = 0;
    {
        status = segment_index_meta.GetNextChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
        if (chunk_id != chunk_info.chunk_id_) {
            UnrecoverableError(fmt::format("Chunk id mismatch, expect: {}, actual: {}", chunk_id, chunk_info.chunk_id_));
        }
        status = segment_index_meta.SetNextChunkID(chunk_id + 1);
        if (!status.ok()) {
            return status;
        }
        status = segment_index_meta.AddChunkID(chunk_id);
        if (!status.ok()) {
            return status;
        }
    }
    ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, segment_index_meta.kv_instance());

    ChunkIndexMetaInfo chunk_meta_info;
    {
        chunk_meta_info.base_name_ = chunk_info.base_name_;
        chunk_meta_info.base_row_id_ = chunk_info.base_rowid_;
        chunk_meta_info.row_cnt_ = chunk_info.row_count_;
        chunk_meta_info.index_size_ = 0;
    }
    status = chunk_index_meta.SetChunkInfo(chunk_meta_info);
    if (!status.ok()) {
        return status;
    }
    status = chunk_index_meta.LoadSet();
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status NewCatalog::CleanChunkIndex(ChunkIndexMeta &chunk_index_meta) {
    Status status;

    status = chunk_index_meta.UninitSet();
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status NewCatalog::GetColumnVector(ColumnMeta &column_meta, SizeT row_count, const ColumnVectorTipe &tipe, ColumnVector &column_vector) {
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
    Status status = column_meta.GetColumnBuffer(buffer_obj, outline_buffer_obj);
    if (!status.ok()) {
        return status;
    }

    column_vector = ColumnVector(col_def->type());
    column_vector.Initialize(buffer_obj, outline_buffer_obj, row_count, tipe);
    return Status::OK();
}

Status NewCatalog::GetBlockVisibleRange(BlockMeta &block_meta, TxnTimeStamp begin_ts, NewTxnGetVisibleRangeState &state) {
    auto [version_buffer, status] = block_meta.GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }

    BufferHandle buffer_handle = version_buffer->Load();
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

Status NewCatalog::GetDBFilePaths(TxnTimeStamp begin_ts, DBMeeta &db_meta, Vector<String> &file_paths) {
    Vector<String> *table_id_strs_ptr = nullptr;
    Status status = db_meta.GetTableIDs(table_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &table_id_str : *table_id_strs_ptr) {
        TableMeeta table_meta(db_meta.db_id_str(), table_id_str, db_meta.kv_instance());
        status = GetTableFilePaths(begin_ts, table_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetTableFilePaths(TxnTimeStamp begin_ts, TableMeeta &table_meta, Vector<String> &file_paths) {
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    Status status;
    std::tie(segment_ids_ptr, status) = table_meta.GetSegmentIDs1(begin_ts);
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, table_meta, table_meta.kv_instance());
        status = GetSegmentFilePaths(begin_ts, segment_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    Vector<String> *index_id_strs_ptr = nullptr;
    status = table_meta.GetIndexIDs(index_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &index_id_str : *index_id_strs_ptr) {
        TableIndexMeeta table_index_meta(index_id_str, table_meta, table_meta.kv_instance());
        status = GetTableIndexFilePaths(table_index_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentFilePaths(TxnTimeStamp begin_ts, SegmentMeta &segment_meta, Vector<String> &file_paths) {
    Vector<BlockID> *block_ids_ptr = nullptr;
    Status status;
    std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1(begin_ts);
    if (!status.ok()) {
        return status;
    }
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta, segment_meta.kv_instance());
        status = GetBlockFilePaths(block_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetBlockFilePaths(BlockMeta &block_meta, Vector<String> &file_paths) {
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    Status status;
    std::tie(column_defs_ptr, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    for (const auto &column_def : *column_defs_ptr) {
        ColumnMeta column_meta(column_def->id(), block_meta, block_meta.kv_instance());
        status = GetBlockColumnFilePaths(column_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    Vector<String> paths = block_meta.FilePaths();
    file_paths.insert(file_paths.end(), std::make_move_iterator(paths.begin()), std::make_move_iterator(paths.end()));
    return Status::OK();
}

Status NewCatalog::GetBlockColumnFilePaths(ColumnMeta &column_meta, Vector<String> &file_paths) {
    Status status;
    Vector<String> paths;
    status = column_meta.FilePaths(paths);
    if (!status.ok()) {
        return status;
    }
    file_paths.insert(file_paths.end(), std::make_move_iterator(paths.begin()), std::make_move_iterator(paths.end()));
    return Status::OK();
}

Status NewCatalog::GetTableIndexFilePaths(TableIndexMeeta &table_index_meta, Vector<String> &file_paths) {
    Status status;
    Vector<SegmentID> *segment_ids_ptr = nullptr;
    status = table_index_meta.GetSegmentIDs(segment_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentIndexMeta segment_index_meta(segment_id, table_index_meta, table_index_meta.kv_instance());
        status = GetSegmentIndexFilepaths(segment_index_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetSegmentIndexFilepaths(SegmentIndexMeta &segment_index_meta, Vector<String> &file_paths) {
    Vector<ChunkID> *chunk_ids_ptr = nullptr;
    Status status = segment_index_meta.GetChunkIDs(chunk_ids_ptr);
    if (!status.ok()) {
        return status;
    }
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta, segment_index_meta.kv_instance());
        status = GetChunkIndexFilePaths(chunk_index_meta, file_paths);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status NewCatalog::GetChunkIndexFilePaths(ChunkIndexMeta &chunk_index_meta, Vector<String> &file_paths) {
    Vector<String> paths;
    Status status = chunk_index_meta.FilePaths(paths);
    if (!status.ok()) {
        return status;
    }
    file_paths.insert(file_paths.end(), std::make_move_iterator(paths.begin()), std::make_move_iterator(paths.end()));
    return Status::OK();
}

} // namespace infinity

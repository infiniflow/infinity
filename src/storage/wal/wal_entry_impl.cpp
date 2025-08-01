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
#include <sstream>
#include <vector>
#include <memory>

module infinity_core:wal_entry.impl;

import :wal_entry;
import :crc;
import serialize;
import :data_block;
import :table_def;
import :index_base;
import :infinity_exception;
import :stl;
import :defer_op;
import :third_party;
import internal_types;
import :logger;
import :block_version;
import :index_defines;
import create_index_info;
import :persistence_manager;
import :infinity_context;
import :virtual_store;
import :chunk_index_meta;
import :table_meeta;
import :segment_meta;
import :block_meta;
import :column_meta;
import :default_values;
import :status;
import statement_common;
import data_type;
import column_def;

namespace infinity {

WalBlockInfo::WalBlockInfo(BlockMeta &block_meta) : block_id_(block_meta.block_id()) {
    Status status;
    // auto [row_count, status] = block_meta.GetRowCnt();
    // if (!status.ok()) {
    //     UnrecoverableError(status.message());
    // }

    // row_count_ = row_count;
    row_capacity_ = block_meta.block_capacity();

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    std::tie(column_defs_ptr, status) = block_meta.segment_meta().table_meta().GetColumnDefs();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    outline_infos_.resize(column_defs_ptr->size());
    Vector<String> paths;
    for (SizeT column_idx = 0; column_idx < column_defs_ptr->size(); ++column_idx) {
        ColumnMeta column_meta(column_idx, block_meta);
        SizeT chunk_offset = 0;
        // status = column_meta.GetChunkOffset(chunk_offset);
        // if (!status.ok()) {
        //     UnrecoverableError(status.message());
        // }
        outline_infos_[column_idx] = {1, chunk_offset};

        Status status = column_meta.FilePaths(paths);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        paths_.insert(paths_.end(), paths.begin(), paths.end());
    }
    paths = block_meta.FilePaths();
    paths_.insert(paths_.end(), paths.begin(), paths.end());

    auto *pm = InfinityContext::instance().persistence_manager();
    addr_serializer_.Initialize(pm, paths_);
#ifdef INFINITY_DEBUG
    for (auto &pth : paths_) {
        assert(!std::filesystem::path(pth).is_absolute());
    }
#endif
}

bool WalBlockInfo::operator==(const WalBlockInfo &other) const {
    return block_id_ == other.block_id_ && row_count_ == other.row_count_ && row_capacity_ == other.row_capacity_ &&
           outline_infos_ == other.outline_infos_;
}

i32 WalBlockInfo::GetSizeInBytes() const {
    i32 size = sizeof(BlockID) + sizeof(row_count_) + sizeof(row_capacity_);
    size += sizeof(i32) + outline_infos_.size() * (sizeof(u32) + sizeof(u64));

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        pm_size_ = addr_serializer_.GetSizeInBytes();
        size += pm_size_;
    }
    return size;
}

void WalBlockInfo::WriteBufferAdv(char *&buf) const {
#ifdef INFINITY_DEBUG
    for (auto &pth : paths_) {
        assert(!std::filesystem::path(pth).is_absolute());
    }
#endif
    WriteBufAdv(buf, block_id_);
    WriteBufAdv(buf, row_count_);
    WriteBufAdv(buf, row_capacity_);
    WriteBufAdv(buf, static_cast<i32>(outline_infos_.size()));
    for (const auto &[idx, off] : outline_infos_) {
        WriteBufAdv(buf, idx);
        WriteBufAdv(buf, off);
    }
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        char *start = buf;
        addr_serializer_.WriteBufAdv(buf);
        SizeT size = buf - start;
        if (size != pm_size_) {
            String error_message = fmt::format("WriteBufferAdv size mismatch: expected {}, actual {}", pm_size_, size);
            UnrecoverableError(error_message);
        }
    }
}

WalBlockInfo WalBlockInfo::ReadBufferAdv(const char *&ptr) {
    WalBlockInfo block_info;
    block_info.block_id_ = ReadBufAdv<BlockID>(ptr);
    block_info.row_count_ = ReadBufAdv<u16>(ptr);
    block_info.row_capacity_ = ReadBufAdv<u16>(ptr);
    i32 count = ReadBufAdv<i32>(ptr);
    block_info.outline_infos_.resize(count);
    for (i32 i = 0; i < count; i++) {
        auto &outline_info = block_info.outline_infos_[i];
        const auto buffer_cnt = ReadBufAdv<u32>(ptr);
        const auto last_chunk_offset = ReadBufAdv<u64>(ptr);
        outline_info = {buffer_cnt, last_chunk_offset};
    }
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        block_info.paths_ = block_info.addr_serializer_.ReadBufAdv(ptr);
    }
    return block_info;
}

String WalBlockInfo::ToString() const {
    std::stringstream ss;
    ss << "block_id: " << block_id_ << ", row_count: " << row_count_ << ", row_capacity: " << row_capacity_;
    ss << ", next_outline_idxes: [";
    for (SizeT i = 0; i < outline_infos_.size(); i++) {
        ss << "outline_buffer_group_" << i << ": (";
        const auto &[idx, off] = outline_infos_[i];
        ss << idx << ", " << off << ")";
        if (i != outline_infos_.size() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return std::move(ss).str();
}

WalSegmentInfo::WalSegmentInfo(SegmentMeta &segment_meta, TxnTimeStamp begin_ts) : segment_id_(segment_meta.segment_id()) {
    Status status;

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs_ptr;
    std::tie(column_defs_ptr, status) = segment_meta.table_meta().GetColumnDefs();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    column_count_ = column_defs_ptr->size();

    Vector<BlockID> *block_ids_ptr = nullptr;
    std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    // SizeT row_count = 0;
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, segment_meta);
        block_infos_.emplace_back(block_meta);

        // SizeT block_row_cnt = 0;
        // std::tie(block_row_cnt, status) = block_meta.GetRowCnt();
        // if (!status.ok()) {
        //     UnrecoverableError(status.message());
        // }
        // row_count += block_row_cnt;
    }

    // row_count_ = row_count;
    // actual_row_count_ = -1; // TODO
    row_capacity_ = segment_meta.segment_capacity();
}

bool WalSegmentInfo::operator==(const WalSegmentInfo &other) const {
    return segment_id_ == other.segment_id_ && column_count_ == other.column_count_ && row_count_ == other.row_count_ &&
           actual_row_count_ == other.actual_row_count_ && row_capacity_ == other.row_capacity_ && block_infos_ == other.block_infos_;
}

i32 WalSegmentInfo::GetSizeInBytes() const {
    i32 size = sizeof(SegmentID) + sizeof(column_count_) + sizeof(row_count_) + sizeof(actual_row_count_) + sizeof(row_capacity_);
    size += sizeof(i32);
    for (const auto &block_info : block_infos_) {
        size += block_info.GetSizeInBytes();
    }
    return size;
}

void WalSegmentInfo::WriteBufferAdv(char *&buf) const {
    WriteBufAdv(buf, segment_id_);
    WriteBufAdv(buf, column_count_);
    WriteBufAdv(buf, row_count_);
    WriteBufAdv(buf, actual_row_count_);
    WriteBufAdv(buf, row_capacity_);
    WriteBufAdv(buf, static_cast<i32>(block_infos_.size()));
    for (const auto &block_info : block_infos_) {
        block_info.WriteBufferAdv(buf);
    }
}

WalSegmentInfo WalSegmentInfo::ReadBufferAdv(const char *&ptr) {
    WalSegmentInfo segment_info;
    segment_info.segment_id_ = ReadBufAdv<SegmentID>(ptr);
    segment_info.column_count_ = ReadBufAdv<u64>(ptr);
    segment_info.row_count_ = ReadBufAdv<SizeT>(ptr);
    segment_info.actual_row_count_ = ReadBufAdv<SizeT>(ptr);
    segment_info.row_capacity_ = ReadBufAdv<SizeT>(ptr);
    i32 count = ReadBufAdv<i32>(ptr);
    for (i32 i = 0; i < count; i++) {
        segment_info.block_infos_.push_back(WalBlockInfo::ReadBufferAdv(ptr));
    }
    return segment_info;
}

String WalSegmentInfo::ToString() const {
    std::stringstream ss;
    ss << "segment_id: " << segment_id_ << ", column_count: " << column_count_ << ", row_count: " << row_count_
       << ", actual_row_count: " << actual_row_count_ << ", row_capacity: " << row_capacity_;
    ss << ", block_info count: " << block_infos_.size() << std::endl;
    return std::move(ss).str();
}

WalChunkIndexInfo::WalChunkIndexInfo(ChunkIndexMeta &chunk_index_meta) : chunk_id_(chunk_index_meta.chunk_id()) {
    ChunkIndexMetaInfo *chunk_info_ptr = nullptr;
    Status status = chunk_index_meta.GetChunkInfo(chunk_info_ptr);
    if (!status.ok()) {
        UnrecoverableError("Failed to get chunk info from chunk index meta.");
    }
    base_name_ = chunk_info_ptr->base_name_;
    base_rowid_ = chunk_info_ptr->base_row_id_;
    row_count_ = chunk_info_ptr->row_cnt_;
    deprecate_ts_ = UNCOMMIT_TS;

    // TODO

    auto *pm = InfinityContext::instance().persistence_manager();
    addr_serializer_.Initialize(pm, paths_);
}

bool WalChunkIndexInfo::operator==(const WalChunkIndexInfo &other) const {
    return chunk_id_ == other.chunk_id_ && base_name_ == other.base_name_ && base_rowid_ == other.base_rowid_ && row_count_ == other.row_count_ &&
           deprecate_ts_ == other.deprecate_ts_;
}

i32 WalChunkIndexInfo::GetSizeInBytes() const {
    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    SizeT size = 0;
    if (use_object_cache) {
        pm_size_ = addr_serializer_.GetSizeInBytes();
        size += pm_size_;
    }
    return size + sizeof(ChunkID) + sizeof(i32) + base_name_.size() + sizeof(base_rowid_) + sizeof(row_count_) + sizeof(deprecate_ts_);
}

void WalChunkIndexInfo::WriteBufferAdv(char *&buf) const {
#ifdef INFINITY_DEBUG
    for (auto &pth : paths_) {
        assert(!std::filesystem::path(pth).is_absolute());
    }
#endif
    WriteBufAdv(buf, chunk_id_);
    WriteBufAdv(buf, base_name_);
    WriteBufAdv(buf, base_rowid_);
    WriteBufAdv(buf, row_count_);
    WriteBufAdv(buf, deprecate_ts_);

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        char *start = buf;
        addr_serializer_.WriteBufAdv(buf);
        SizeT size = buf - start;
        if (size != pm_size_) {
            String error_message = fmt::format("WriteBufferAdv size mismatch: expected {}, actual {}", pm_size_, size);
            UnrecoverableError(error_message);
        }
    }
}

WalChunkIndexInfo WalChunkIndexInfo::ReadBufferAdv(const char *&ptr) {
    WalChunkIndexInfo chunk_index_info;
    chunk_index_info.chunk_id_ = ReadBufAdv<ChunkID>(ptr);
    chunk_index_info.base_name_ = ReadBufAdv<String>(ptr);
    chunk_index_info.base_rowid_ = ReadBufAdv<RowID>(ptr);
    chunk_index_info.row_count_ = ReadBufAdv<u32>(ptr);
    chunk_index_info.deprecate_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);

    PersistenceManager *pm = InfinityContext::instance().persistence_manager();
    bool use_object_cache = pm != nullptr;
    if (use_object_cache) {
        chunk_index_info.paths_ = chunk_index_info.addr_serializer_.ReadBufAdv(ptr);
    }
    return chunk_index_info;
}

String WalChunkIndexInfo::ToString() const {
    std::stringstream ss;
    ss << "chunk_id: " << chunk_id_ << ", base_name: " << base_name_ << ", base_rowid: " << base_rowid_.ToString() << ", row_count: " << row_count_
       << ", deprecate_ts: " << deprecate_ts_;
    return std::move(ss).str();
}

bool WalSegmentIndexInfo::operator==(const WalSegmentIndexInfo &other) const = default;

i32 WalSegmentIndexInfo::GetSizeInBytes() const {
    i32 size = sizeof(SegmentID) + sizeof(i32);
    for (const auto &chunk_info : chunk_infos_) {
        size += chunk_info.GetSizeInBytes();
    }
    return size;
}

void WalSegmentIndexInfo::WriteBufferAdv(char *&buf) const {
    WriteBufAdv(buf, segment_id_);
    WriteBufAdv(buf, static_cast<i32>(chunk_infos_.size()));
    for (const auto &chunk_info : chunk_infos_) {
        chunk_info.WriteBufferAdv(buf);
    }
}

WalSegmentIndexInfo WalSegmentIndexInfo::ReadBufferAdv(const char *&ptr) {
    SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
    i32 count = ReadBufAdv<i32>(ptr);
    WalSegmentIndexInfo segment_index_info;
    segment_index_info.segment_id_ = segment_id;
    for (i32 i = 0; i < count; i++) {
        segment_index_info.chunk_infos_.push_back(WalChunkIndexInfo::ReadBufferAdv(ptr));
    }

    return segment_index_info;
}

String WalSegmentIndexInfo::ToString() const {
    std::stringstream ss;
    ss << "segment_id: " << segment_id_ << ", chunk_info count: " << chunk_infos_.size() << std::endl;
    return std::move(ss).str();
}

SharedPtr<WalCmd> WalCmd::ReadAdv(const char *&ptr, i32 max_bytes) {
    const char *const ptr_end = ptr + max_bytes;
    SharedPtr<WalCmd> cmd = nullptr;
    auto cmd_type = ReadBufAdv<WalCommandType>(ptr);
    switch (cmd_type) {
        case WalCommandType::CREATE_DATABASE: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_dir_tail = ReadBufAdv<String>(ptr);
            String db_comment = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdCreateDatabase>(db_name, db_dir_tail, db_comment);
            break;
        }
        case WalCommandType::CREATE_DATABASE_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String db_comment = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdCreateDatabaseV2>(db_name, db_id, db_comment);
            break;
        }
        case WalCommandType::DROP_DATABASE: {
            String db_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropDatabase>(db_name);
            break;
        }
        case WalCommandType::DROP_DATABASE_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            TxnTimeStamp create_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            cmd = MakeShared<WalCmdDropDatabaseV2>(db_name, db_id, create_ts);
            break;
        }
        case WalCommandType::CREATE_TABLE: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_dir_tail = ReadBufAdv<String>(ptr);
            SharedPtr<TableDef> table_def = TableDef::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdCreateTable>(db_name, table_dir_tail, table_def);
            break;
        }
        case WalCommandType::CREATE_TABLE_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            SharedPtr<TableDef> table_def = TableDef::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdCreateTableV2>(db_name, db_id, table_id, table_def);
            break;
        }
        case WalCommandType::DROP_TABLE: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropTable>(db_name, table_name);
            break;
        }
        case WalCommandType::DROP_TABLE_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            TxnTimeStamp create_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            String table_key = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropTableV2>(db_name, db_id, table_name, table_id, create_ts, table_key);
            break;
        }
        case WalCommandType::IMPORT: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            WalSegmentInfo segment_info = WalSegmentInfo::ReadBufferAdv(ptr);
            cmd = MakeShared<WalCmdImport>(db_name, table_name, segment_info);
            break;
        }
        case WalCommandType::IMPORT_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            WalSegmentInfo segment_info = WalSegmentInfo::ReadBufferAdv(ptr);
            cmd = MakeShared<WalCmdImportV2>(db_name, db_id, table_name, table_id, segment_info);
            break;
        }
        case WalCommandType::APPEND: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SharedPtr<DataBlock> block = DataBlock::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdAppend>(db_name, table_name, block);
            break;
        }
        case WalCommandType::APPEND_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            i32 count = ReadBufAdv<i32>(ptr);
            Vector<Pair<RowID, u64>> row_ranges(count);
            for (auto &row_range : row_ranges) {
                row_range.first = ReadBufAdv<RowID>(ptr);
                row_range.second = ReadBufAdv<u64>(ptr);
            }
            SharedPtr<DataBlock> block = DataBlock::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdAppendV2>(std::move(db_name),
                                             std::move(db_id),
                                             std::move(table_name),
                                             std::move(table_id),
                                             std::move(row_ranges),
                                             std::move(block));
            break;
        }
        case WalCommandType::DELETE: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            i32 cnt = ReadBufAdv<i32>(ptr);
            Vector<RowID> row_ids;
            for (i32 i = 0; i < cnt; ++i) {
                auto row_id = ReadBufAdv<RowID>(ptr);
                row_ids.push_back(row_id);
            }
            cmd = MakeShared<WalCmdDelete>(db_name, table_name, row_ids);
            break;
        }
        case WalCommandType::DELETE_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            i32 cnt = ReadBufAdv<i32>(ptr);
            Vector<RowID> row_ids;
            for (i32 i = 0; i < cnt; ++i) {
                auto row_id = ReadBufAdv<RowID>(ptr);
                row_ids.push_back(row_id);
            }
            cmd = MakeShared<WalCmdDeleteV2>(db_name, db_id, table_name, table_id, row_ids);
            break;
        }
        case WalCommandType::SET_SEGMENT_STATUS_SEALED: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            String segment_filter_binary_data = ReadBufAdv<String>(ptr);
            i32 count = ReadBufAdv<i32>(ptr);
            Vector<Pair<BlockID, String>> block_filter_binary_data(count);
            for (auto &data : block_filter_binary_data) {
                data.first = ReadBufAdv<BlockID>(ptr);
                data.second = ReadBufAdv<String>(ptr);
            }
            cmd = MakeShared<WalCmdSetSegmentStatusSealed>(db_name, table_name, segment_id, segment_filter_binary_data, block_filter_binary_data);
            break;
        }
        case WalCommandType::SET_SEGMENT_STATUS_SEALED_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            String segment_filter_binary_data = ReadBufAdv<String>(ptr);
            i32 count = ReadBufAdv<i32>(ptr);
            Vector<Pair<BlockID, String>> block_filter_binary_data(count);
            for (auto &data : block_filter_binary_data) {
                data.first = ReadBufAdv<BlockID>(ptr);
                data.second = ReadBufAdv<String>(ptr);
            }
            cmd = MakeShared<WalCmdSetSegmentStatusSealedV2>(db_name,
                                                             db_id,
                                                             table_name,
                                                             table_id,
                                                             segment_id,
                                                             segment_filter_binary_data,
                                                             block_filter_binary_data);
            break;
        }
        case WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            String segment_filter_binary_data = ReadBufAdv<String>(ptr);
            i32 count = ReadBufAdv<i32>(ptr);
            Vector<Pair<BlockID, String>> block_filter_binary_data(count);
            for (auto &data : block_filter_binary_data) {
                data.first = ReadBufAdv<BlockID>(ptr);
                data.second = ReadBufAdv<String>(ptr);
            }
            cmd =
                MakeShared<WalCmdUpdateSegmentBloomFilterData>(db_name, table_name, segment_id, segment_filter_binary_data, block_filter_binary_data);
            break;
        }
        case WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            String segment_filter_binary_data = ReadBufAdv<String>(ptr);
            i32 count = ReadBufAdv<i32>(ptr);
            Vector<Pair<BlockID, String>> block_filter_binary_data(count);
            for (auto &data : block_filter_binary_data) {
                data.first = ReadBufAdv<BlockID>(ptr);
                data.second = ReadBufAdv<String>(ptr);
            }
            cmd = MakeShared<WalCmdUpdateSegmentBloomFilterDataV2>(db_name,
                                                                   db_id,
                                                                   table_name,
                                                                   table_id,
                                                                   segment_id,
                                                                   segment_filter_binary_data,
                                                                   block_filter_binary_data);
            break;
        }
        case WalCommandType::CHECKPOINT: {
            i64 max_commit_ts = ReadBufAdv<i64>(ptr);
            String catalog_path = ReadBufAdv<String>(ptr);
            String catalog_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdCheckpoint>(max_commit_ts, catalog_path, catalog_name);
            break;
        }
        case WalCommandType::CHECKPOINT_V2: {
            i64 max_commit_ts = ReadBufAdv<i64>(ptr);
            cmd = MakeShared<WalCmdCheckpointV2>(max_commit_ts);
            break;
        }
        case WalCommandType::CREATE_INDEX: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_dir_tail = ReadBufAdv<String>(ptr);
            SharedPtr<IndexBase> index_base = IndexBase::ReadAdv(ptr, ptr_end - ptr);
            auto create_index_cmd = MakeShared<WalCmdCreateIndex>(db_name, table_name, index_dir_tail, index_base);
            i32 segment_info_n = ReadBufAdv<i32>(ptr);
            for (i32 i = 0; i < segment_info_n; ++i) {
                WalSegmentIndexInfo segment_index_info = WalSegmentIndexInfo::ReadBufferAdv(ptr);
                create_index_cmd->segment_index_infos_.push_back(std::move(segment_index_info));
            }
            cmd = std::move(create_index_cmd);
            break;
        }
        case WalCommandType::CREATE_INDEX_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            String index_id = ReadBufAdv<String>(ptr);
            SharedPtr<IndexBase> index_base = IndexBase::ReadAdv(ptr, ptr_end - ptr);
            Vector<WalSegmentIndexInfo> segment_index_infos;
            i32 segment_info_n = ReadBufAdv<i32>(ptr);
            for (i32 i = 0; i < segment_info_n; ++i) {
                WalSegmentIndexInfo segment_index_info = WalSegmentIndexInfo::ReadBufferAdv(ptr);
                segment_index_infos.push_back(std::move(segment_index_info));
            }
            String table_key = ReadBufAdv<String>(ptr);
            auto create_index_cmd = MakeShared<WalCmdCreateIndexV2>(db_name, db_id, table_name, table_id, index_id, index_base, table_key);
            create_index_cmd->segment_index_infos_ = std::move(segment_index_infos);
            cmd = std::move(create_index_cmd);
            break;
        }
        case WalCommandType::DROP_INDEX: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropIndex>(db_name, table_name, index_name);
            break;
        }
        case WalCommandType::DROP_INDEX_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String index_id = ReadBufAdv<String>(ptr);
            TxnTimeStamp create_ts = ReadBufAdv<TxnTimeStamp>(ptr);
            String index_key = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropIndexV2>(db_name, db_id, table_name, table_id, index_name, index_id, create_ts, index_key);
            break;
        }
        case WalCommandType::COMPACT: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            i32 new_segment_n = ReadBufAdv<i32>(ptr);
            Vector<WalSegmentInfo> new_segment_infos;
            for (i32 i = 0; i < new_segment_n; ++i) {
                new_segment_infos.push_back(WalSegmentInfo::ReadBufferAdv(ptr));
            }
            i32 deprecated_n = ReadBufAdv<i32>(ptr);
            Vector<SegmentID> deprecated_segment_ids;
            for (i32 i = 0; i < deprecated_n; ++i) {
                SegmentID deprecated_segment_id = ReadBufAdv<SegmentID>(ptr);
                deprecated_segment_ids.push_back(deprecated_segment_id);
            }
            cmd = MakeShared<WalCmdCompact>(db_name, table_name, new_segment_infos, deprecated_segment_ids);
            break;
        }
        case WalCommandType::COMPACT_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            i32 new_segment_n = ReadBufAdv<i32>(ptr);
            Vector<WalSegmentInfo> new_segment_infos;
            for (i32 i = 0; i < new_segment_n; ++i) {
                new_segment_infos.push_back(WalSegmentInfo::ReadBufferAdv(ptr));
            }
            i32 deprecated_n = ReadBufAdv<i32>(ptr);
            Vector<SegmentID> deprecated_segment_ids;
            for (i32 i = 0; i < deprecated_n; ++i) {
                SegmentID deprecated_segment_id = ReadBufAdv<SegmentID>(ptr);
                deprecated_segment_ids.push_back(deprecated_segment_id);
            }
            cmd = MakeShared<WalCmdCompactV2>(db_name, db_id, table_name, table_id, new_segment_infos, deprecated_segment_ids);
            break;
        }
        case WalCommandType::OPTIMIZE: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            auto param_n = ReadBufAdv<i32>(ptr);
            Vector<UniquePtr<InitParameter>> params;
            for (i32 i = 0; i < param_n; i++) {
                params.push_back(InitParameter::ReadAdv(ptr));
            }
            cmd = MakeShared<WalCmdOptimize>(db_name, table_name, index_name, std::move(params));
            break;
        }
        case WalCommandType::OPTIMIZE_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String index_id = ReadBufAdv<String>(ptr);
            auto param_n = ReadBufAdv<i32>(ptr);
            Vector<UniquePtr<InitParameter>> params;
            for (i32 i = 0; i < param_n; i++) {
                params.push_back(InitParameter::ReadAdv(ptr));
            }
            cmd = MakeShared<WalCmdOptimizeV2>(db_name, db_id, table_name, table_id, index_name, index_id, std::move(params));
            break;
        }
        case WalCommandType::DUMP_INDEX: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            i32 chunk_n = ReadBufAdv<i32>(ptr);
            Vector<WalChunkIndexInfo> chunk_infos;
            for (i32 i = 0; i < chunk_n; ++i) {
                chunk_infos.push_back(WalChunkIndexInfo::ReadBufferAdv(ptr));
            }
            i32 old_chunk_n = ReadBufAdv<i32>(ptr);
            Vector<ChunkID> old_chunk_ids;
            for (i32 i = 0; i < old_chunk_n; ++i) {
                old_chunk_ids.push_back(ReadBufAdv<ChunkID>(ptr));
            }
            cmd = MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id, chunk_infos, old_chunk_ids);
            break;
        }
        case WalCommandType::DUMP_INDEX_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            String index_id = ReadBufAdv<String>(ptr);
            SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
            i32 chunk_n = ReadBufAdv<i32>(ptr);
            Vector<WalChunkIndexInfo> chunk_infos;
            for (i32 i = 0; i < chunk_n; ++i) {
                chunk_infos.push_back(WalChunkIndexInfo::ReadBufferAdv(ptr));
            }
            i32 old_chunk_n = ReadBufAdv<i32>(ptr);
            Vector<ChunkID> old_chunk_ids;
            for (i32 i = 0; i < old_chunk_n; ++i) {
                old_chunk_ids.push_back(ReadBufAdv<ChunkID>(ptr));
            }
            String table_key = ReadBufAdv<String>(ptr);
            cmd = MakeShared<
                WalCmdDumpIndexV2>(db_name, db_id, table_name, table_id, index_name, index_id, segment_id, chunk_infos, old_chunk_ids, table_key);
            break;
        }
        case WalCommandType::RENAME_TABLE: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String new_table_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdRenameTable>(db_name, table_name, new_table_name);
            break;
        }
        case WalCommandType::RENAME_TABLE_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            String new_table_name = ReadBufAdv<String>(ptr);
            String old_table_key = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdRenameTableV2>(db_name, db_id, table_name, table_id, new_table_name, old_table_key);
            break;
        }
        case WalCommandType::ADD_COLUMNS: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            i32 column_n = ReadBufAdv<i32>(ptr);
            Vector<SharedPtr<ColumnDef>> columns;
            for (i32 i = 0; i < column_n; i++) {
                auto cd = ColumnDef::ReadAdv(ptr, max_bytes);
                columns.push_back(cd);
            }
            cmd = MakeShared<WalCmdAddColumns>(db_name, table_name, columns);
            break;
        }
        case WalCommandType::ADD_COLUMNS_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            i32 column_n = ReadBufAdv<i32>(ptr);
            Vector<SharedPtr<ColumnDef>> columns;
            for (i32 i = 0; i < column_n; i++) {
                auto cd = ColumnDef::ReadAdv(ptr, max_bytes);
                columns.push_back(cd);
            }
            String table_key = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdAddColumnsV2>(db_name, db_id, table_name, table_id, columns, table_key);
            break;
        }
        case WalCommandType::DROP_COLUMNS: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            i32 column_n = ReadBufAdv<i32>(ptr);
            Vector<String> column_names;
            for (i32 i = 0; i < column_n; i++) {
                column_names.push_back(ReadBufAdv<String>(ptr));
            }
            cmd = MakeShared<WalCmdDropColumns>(db_name, table_name, column_names);
            break;
        }
        case WalCommandType::DROP_COLUMNS_V2: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_id = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_id = ReadBufAdv<String>(ptr);
            i32 column_n = ReadBufAdv<i32>(ptr);
            Vector<String> column_names;
            for (i32 i = 0; i < column_n; i++) {
                column_names.push_back(ReadBufAdv<String>(ptr));
            }
            column_n = ReadBufAdv<i32>(ptr);
            Vector<ColumnID> column_ids;
            for (i32 i = 0; i < column_n; i++) {
                column_ids.push_back(ReadBufAdv<ColumnID>(ptr));
            }
            String table_key = ReadBufAdv<String>(ptr);
            column_n = ReadBufAdv<i32>(ptr);
            Vector<String> column_keys;
            for (i32 i = 0; i < column_n; i++) {
                column_keys.push_back(ReadBufAdv<String>(ptr));
            }
            cmd = MakeShared<WalCmdDropColumnsV2>(db_name, db_id, table_name, table_id, column_names, column_ids, table_key, column_keys);
            break;
        }
        case WalCommandType::CLEANUP: {
            i64 timestamp = ReadBufAdv<i64>(ptr);
            cmd = MakeShared<WalCmdCleanup>(timestamp);
            break;
        }
        default: {
            String error_message = fmt::format("UNIMPLEMENTED ReadAdv for WAL command {}", int(cmd_type));
            UnrecoverableError(error_message);
        }
    }
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        String error_message = fmt::format("ptr goes out of range when reading WalCmd: {}", WalCmd::WalCommandTypeToString(cmd_type));
        UnrecoverableError(error_message);
    }
    return cmd;
}

bool WalCmdCreateDatabase::operator==(const WalCmd &other) const {
    const auto *other_cmd = dynamic_cast<const WalCmdCreateDatabase *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_dir_tail_ == other_cmd->db_dir_tail_;
}

bool WalCmdCreateDatabaseV2::operator==(const WalCmd &other) const {
    const auto *other_cmd = dynamic_cast<const WalCmdCreateDatabaseV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_;
}

bool WalCmdDropDatabase::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropDatabase *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_;
}

bool WalCmdDropDatabaseV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropDatabaseV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && create_ts_ == other_cmd->create_ts_;
}

bool WalCmdCreateTable::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateTable *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_dir_tail_ == other_cmd->table_dir_tail_ && table_def_.get() != nullptr &&
           other_cmd->table_def_.get() != nullptr && *table_def_ == *other_cmd->table_def_;
}

bool WalCmdCreateTableV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateTableV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_id_ == other_cmd->table_id_ &&
           table_def_.get() != nullptr && other_cmd->table_def_.get() != nullptr && *table_def_ == *other_cmd->table_def_;
}

bool WalCmdDropTable::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropTable *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_;
}

bool WalCmdDropTableV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropTableV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_name_ == other_cmd->table_name_ &&
           table_id_ == other_cmd->table_id_ && create_ts_ == other_cmd->create_ts_ && table_key_ == other_cmd->table_key_;
}

bool WalCmdCreateIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ &&
           index_dir_tail_ == other_cmd->index_dir_tail_ && index_base_.get() != nullptr && other_cmd->index_base_.get() != nullptr &&
           *index_base_ == *other_cmd->index_base_ && segment_index_infos_ == other_cmd->segment_index_infos_;
}

bool WalCmdCreateIndexV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateIndexV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_name_ == other_cmd->table_name_ &&
           table_id_ == other_cmd->table_id_ && index_base_.get() != nullptr && other_cmd->index_base_.get() != nullptr &&
           *index_base_ == *other_cmd->index_base_ && segment_index_infos_ == other_cmd->segment_index_infos_ && table_key_ == other_cmd->table_key_;
}

bool WalCmdDropIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ && index_name_ == other_cmd->index_name_;
}

bool WalCmdDropIndexV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropIndexV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_name_ == other_cmd->table_name_ &&
           table_id_ == other_cmd->table_id_ && index_name_ == other_cmd->index_name_ && create_ts_ == other_cmd->create_ts_ &&
           index_key_ == other_cmd->index_key_;
}

bool WalCmdImport::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdImport *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || table_name_ != other_cmd->table_name_ ||
        segment_info_ != other_cmd->segment_info_) {
        return false;
    }
    return true;
}

bool WalCmdImportV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdImportV2 *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || db_id_ != other_cmd->db_id_ || table_name_ != other_cmd->table_name_ ||
        table_id_ != other_cmd->table_id_ || segment_info_ != other_cmd->segment_info_) {
        return false;
    }
    return true;
}

bool WalCmdAppend::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdAppend *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || table_name_ != other_cmd->table_name_)
        return false;
    return true;
}

bool WalCmdAppendV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdAppendV2 *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || db_id_ != other_cmd->db_id_ || table_name_ != other_cmd->table_name_ ||
        table_id_ != other_cmd->table_id_ || row_ranges_.size() != other_cmd->row_ranges_.size())
        return false;
    for (SizeT i = 0; i < row_ranges_.size(); i++) {
        if (row_ranges_[i].first != other_cmd->row_ranges_[i].first || row_ranges_[i].second != other_cmd->row_ranges_[i].second) {
            return false;
        }
    }
    return true;
}

bool WalCmdDelete::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDelete *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || table_name_ != other_cmd->table_name_ ||
        row_ids_.size() != other_cmd->row_ids_.size()) {
        return false;
    }
    for (SizeT i = 0; i < row_ids_.size(); i++) {
        if (row_ids_[i] != other_cmd->row_ids_[i]) {
            return false;
        }
    }
    return true;
}

bool WalCmdDeleteV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDeleteV2 *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || db_id_ != other_cmd->db_id_ || table_name_ != other_cmd->table_name_ ||
        table_id_ != other_cmd->table_id_ || row_ids_.size() != other_cmd->row_ids_.size()) {
        return false;
    }
    for (SizeT i = 0; i < row_ids_.size(); i++) {
        if (row_ids_[i] != other_cmd->row_ids_[i]) {
            return false;
        }
    }
    return true;
}

bool WalCmdSetSegmentStatusSealed::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdSetSegmentStatusSealed *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || table_name_ != other_cmd->table_name_ || segment_id_ != other_cmd->segment_id_ ||
        segment_filter_binary_data_ != other_cmd->segment_filter_binary_data_) {
        return false;
    }
    if (block_filter_binary_data_ != other_cmd->block_filter_binary_data_) {
        return false;
    }
    return true;
}

bool WalCmdSetSegmentStatusSealedV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdSetSegmentStatusSealedV2 *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || db_id_ != other_cmd->db_id_ || table_name_ != other_cmd->table_name_ ||
        table_id_ != other_cmd->table_id_ || segment_id_ != other_cmd->segment_id_ ||
        segment_filter_binary_data_ != other_cmd->segment_filter_binary_data_) {
        return false;
    }
    if (block_filter_binary_data_ != other_cmd->block_filter_binary_data_) {
        return false;
    }
    return true;
}

bool WalCmdUpdateSegmentBloomFilterData::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdUpdateSegmentBloomFilterData *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || table_name_ != other_cmd->table_name_ || segment_id_ != other_cmd->segment_id_ ||
        segment_filter_binary_data_ != other_cmd->segment_filter_binary_data_) {
        return false;
    }
    if (block_filter_binary_data_ != other_cmd->block_filter_binary_data_) {
        return false;
    }
    return true;
}

bool WalCmdUpdateSegmentBloomFilterDataV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdUpdateSegmentBloomFilterDataV2 *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || db_id_ != other_cmd->db_id_ || table_name_ != other_cmd->table_name_ ||
        table_id_ != other_cmd->table_id_ || segment_id_ != other_cmd->segment_id_ ||
        segment_filter_binary_data_ != other_cmd->segment_filter_binary_data_) {
        return false;
    }
    if (block_filter_binary_data_ != other_cmd->block_filter_binary_data_) {
        return false;
    }
    return true;
}

bool WalCmdCheckpoint::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCheckpoint *>(&other);
    return other_cmd != nullptr && max_commit_ts_ == other_cmd->max_commit_ts_;
}

bool WalCmdCheckpointV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCheckpointV2 *>(&other);
    return other_cmd != nullptr && max_commit_ts_ == other_cmd->max_commit_ts_;
}

bool WalCmdCompact::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCompact *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || table_name_ != other_cmd->table_name_ ||
        deprecated_segment_ids_.size() != other_cmd->deprecated_segment_ids_.size()) {
        return false;
    }
    for (SizeT i = 0; i < deprecated_segment_ids_.size(); i++) {
        if (deprecated_segment_ids_[i] != other_cmd->deprecated_segment_ids_[i]) {
            return false;
        }
    }
    return true;
}

bool WalCmdCompactV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCompactV2 *>(&other);
    if (other_cmd == nullptr || db_name_ != other_cmd->db_name_ || db_id_ != other_cmd->db_id_ || table_name_ != other_cmd->table_name_ ||
        table_id_ != other_cmd->table_id_ || deprecated_segment_ids_.size() != other_cmd->deprecated_segment_ids_.size()) {
        return false;
    }
    for (SizeT i = 0; i < deprecated_segment_ids_.size(); i++) {
        if (deprecated_segment_ids_[i] != other_cmd->deprecated_segment_ids_[i]) {
            return false;
        }
    }
    return true;
}

bool WalCmdOptimize::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdOptimize *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ && index_name_ == other_cmd->index_name_;
}

bool WalCmdOptimizeV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdOptimizeV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_name_ == other_cmd->table_name_ &&
           table_id_ == other_cmd->table_id_ && index_name_ == other_cmd->index_name_ && index_id_ == other_cmd->index_id_ &&
           params_ == other_cmd->params_;
}

bool WalCmdDumpIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDumpIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ &&
           index_name_ == other_cmd->index_name_ && segment_id_ == other_cmd->segment_id_ && chunk_infos_ == other_cmd->chunk_infos_ &&
           deprecate_ids_ == other_cmd->deprecate_ids_;
}

bool WalCmdDumpIndexV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDumpIndexV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_name_ == other_cmd->table_name_ &&
           index_name_ == other_cmd->index_name_ && segment_id_ == other_cmd->segment_id_ && chunk_infos_ == other_cmd->chunk_infos_ &&
           deprecate_ids_ == other_cmd->deprecate_ids_ && table_key_ == other_cmd->table_key_;
}

bool WalCmdRenameTable::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdRenameTable *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ &&
           new_table_name_ == other_cmd->new_table_name_;
}

bool WalCmdRenameTableV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdRenameTableV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_name_ == other_cmd->table_name_ &&
           table_id_ == other_cmd->table_id_ && new_table_name_ == other_cmd->new_table_name_ && old_table_key_ == other_cmd->old_table_key_;
}

bool WalCmdAddColumns::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdAddColumns *>(&other);
    bool res = other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ &&
               column_defs_.size() == other_cmd->column_defs_.size();
    if (!res) {
        return false;
    }
    for (SizeT i = 0; i < column_defs_.size(); i++) {
        if (*(column_defs_[i]) != *(other_cmd->column_defs_[i])) {
            return false;
        }
    }
    return true;
}

bool WalCmdAddColumnsV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdAddColumnsV2 *>(&other);
    bool res = other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_name_ == other_cmd->table_name_ &&
               table_id_ == other_cmd->table_id_ && column_defs_.size() == other_cmd->column_defs_.size() && table_key_ == other_cmd->table_key_;
    if (!res) {
        return false;
    }
    for (SizeT i = 0; i < column_defs_.size(); i++) {
        if (*(column_defs_[i]) != *(other_cmd->column_defs_[i])) {
            return false;
        }
    }
    return true;
}

bool WalCmdDropColumns::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropColumns *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ &&
           column_names_ == other_cmd->column_names_;
}

bool WalCmdDropColumnsV2::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropColumnsV2 *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && db_id_ == other_cmd->db_id_ && table_name_ == other_cmd->table_name_ &&
           table_id_ == other_cmd->table_id_ && column_names_ == other_cmd->column_names_ && column_ids_ == other_cmd->column_ids_ &&
           table_key_ == other_cmd->table_key_ && column_keys_ == other_cmd->column_keys_;
}

bool WalCmdCleanup::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCleanup *>(&other);
    return other_cmd != nullptr && timestamp_ == other_cmd->timestamp_;
}

i32 WalCmdCreateDatabase::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_dir_tail_.size() + sizeof(i32) +
           this->db_comment_.size();
}

i32 WalCmdCreateDatabaseV2::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(i32) + this->db_comment_.size();
}

i32 WalCmdDropDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size(); }

i32 WalCmdDropDatabaseV2::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(TxnTimeStamp);
}

i32 WalCmdCreateTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_dir_tail_.size() +
           this->table_def_->GetSizeInBytes();
}

i32 WalCmdCreateTableV2::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(i32) + this->table_id_.size() +
           this->table_def_->GetSizeInBytes();
}

i32 WalCmdCreateIndex::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
               this->index_dir_tail_.size() + this->index_base_->GetSizeInBytes();
    size += sizeof(i32);
    for (const auto &segment_index_info : segment_index_infos_) {
        size += segment_index_info.GetSizeInBytes();
    }
    return size;
}

i32 WalCmdCreateIndexV2::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(i32) +
               this->table_name_.size() + sizeof(i32) + this->table_id_.size() + sizeof(i32) + this->index_id_.size() +
               this->index_base_->GetSizeInBytes();
    size += sizeof(i32);
    for (const auto &segment_index_info : segment_index_infos_) {
        size += segment_index_info.GetSizeInBytes();
    }
    size += sizeof(i32) + this->table_key_.size();
    return size;
}

i32 WalCmdDropTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size();
}

i32 WalCmdDropTableV2::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(i32) + this->table_name_.size() +
           sizeof(i32) + this->table_id_.size() + sizeof(TxnTimeStamp) + sizeof(i32) + this->table_key_.size();
}

i32 WalCmdDropIndex::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
           this->index_name_.size();
}

i32 WalCmdDropIndexV2::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(i32) + this->table_name_.size() +
           sizeof(i32) + this->table_id_.size() + sizeof(i32) + this->index_name_.size() + sizeof(i32) + this->index_id_.size() +
           sizeof(TxnTimeStamp) + sizeof(i32) + this->index_key_.size();
}

i32 WalCmdImport::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + segment_info_.GetSizeInBytes();
}

i32 WalCmdImportV2::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(i32) + this->table_name_.size() +
           sizeof(i32) + this->table_id_.size() + segment_info_.GetSizeInBytes();
}

i32 WalCmdAppend::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + block_->GetSizeInBytes();
}

i32 WalCmdAppendV2::GetSizeInBytes() const {
    i32 sz = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(i32) +
             this->table_name_.size() + sizeof(i32) + this->table_id_.size() + block_->GetSizeInBytes();
    sz += sizeof(i32) + this->row_ranges_.size() * (sizeof(RowID) + sizeof(u64));
    return sz;
}

i32 WalCmdDelete::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
           row_ids_.size() * sizeof(RowID);
}

i32 WalCmdDeleteV2::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_id_.size() + sizeof(i32) + this->table_name_.size() +
           sizeof(i32) + this->table_id_.size() + sizeof(i32) + row_ids_.size() * sizeof(RowID);
}

i32 WalCmdSetSegmentStatusSealed::GetSizeInBytes() const {
    i32 sz = sizeof(WalCommandType) + ::infinity::GetSizeInBytes(db_name_) + ::infinity::GetSizeInBytes(table_name_) +
             ::infinity::GetSizeInBytes(segment_id_) + ::infinity::GetSizeInBytes(segment_filter_binary_data_);
    sz += +sizeof(i32); // count of vector
    for (const auto &data : block_filter_binary_data_) {
        sz += sizeof(data.first) + ::infinity::GetSizeInBytes(data.second);
    }
    return sz;
}

i32 WalCmdSetSegmentStatusSealedV2::GetSizeInBytes() const {
    i32 sz = sizeof(WalCommandType) + sizeof(i32) + db_name_.size() + sizeof(i32) + db_id_.size() + sizeof(i32) + table_name_.size() + sizeof(i32) +
             table_id_.size() + sizeof(segment_id_) + sizeof(i32) + segment_filter_binary_data_.size();
    sz += +sizeof(i32); // count of vector
    for (const auto &data : block_filter_binary_data_) {
        sz += sizeof(data.first) + sizeof(i32) + data.second.size();
    }
    return sz;
}

i32 WalCmdUpdateSegmentBloomFilterData::GetSizeInBytes() const {
    i32 sz = sizeof(WalCommandType) + ::infinity::GetSizeInBytes(db_name_) + ::infinity::GetSizeInBytes(table_name_) +
             ::infinity::GetSizeInBytes(segment_id_) + ::infinity::GetSizeInBytes(segment_filter_binary_data_);
    sz += +sizeof(i32); // count of vector
    for (const auto &data : block_filter_binary_data_) {
        sz += sizeof(data.first) + ::infinity::GetSizeInBytes(data.second);
    }
    return sz;
}

i32 WalCmdUpdateSegmentBloomFilterDataV2::GetSizeInBytes() const {
    i32 sz = sizeof(WalCommandType) + sizeof(i32) + db_name_.size() + sizeof(i32) + db_id_.size() + sizeof(i32) + table_name_.size() + sizeof(i32) +
             table_id_.size() + sizeof(segment_id_) + sizeof(i32) + segment_filter_binary_data_.size();
    sz += +sizeof(i32); // count of vector
    for (const auto &data : block_filter_binary_data_) {
        sz += sizeof(data.first) + ::infinity::GetSizeInBytes(data.second);
    }
    return sz;
}

i32 WalCmdCheckpoint::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(max_commit_ts_) + sizeof(i32) + catalog_path_.size() + sizeof(i32) + catalog_name_.size();
}

i32 WalCmdCheckpointV2::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(max_commit_ts_); }

i32 WalCmdCompact::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32);
    for (const auto &segment_info : this->new_segment_infos_) {
        size += segment_info.GetSizeInBytes();
    }
    return size + sizeof(i32) + this->deprecated_segment_ids_.size() * sizeof(SegmentID);
}

i32 WalCmdCompactV2::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + db_name_.size() + sizeof(i32) + db_id_.size() + sizeof(i32) + table_name_.size() + sizeof(i32) +
               table_id_.size() + sizeof(i32);
    for (const auto &segment_info : this->new_segment_infos_) {
        size += segment_info.GetSizeInBytes();
    }
    return size + sizeof(i32) + this->deprecated_segment_ids_.size() * sizeof(SegmentID);
}

i32 WalCmdOptimize::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
               this->index_name_.size();
    size += sizeof(i32);
    for (const auto &param : this->params_) {
        size += param->GetSizeInBytes();
    }
    return size;
}

i32 WalCmdOptimizeV2::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + db_name_.size() + sizeof(i32) + db_id_.size() + sizeof(i32) + table_name_.size() + sizeof(i32) +
               table_id_.size() + sizeof(i32) + index_name_.size() + sizeof(i32) + index_id_.size();
    size += sizeof(i32);
    for (const auto &param : this->params_) {
        size += param->GetSizeInBytes();
    }
    return size;
}

i32 WalCmdDumpIndex::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
               this->index_name_.size() + sizeof(SegmentID) + sizeof(i32);
    for (const auto &chunk_info : this->chunk_infos_) {
        size += chunk_info.GetSizeInBytes();
    }
    size += sizeof(i32) + sizeof(ChunkID) * this->deprecate_ids_.size();
    return size;
}

i32 WalCmdDumpIndexV2::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + db_name_.size() + sizeof(i32) + db_id_.size() + sizeof(i32) + table_name_.size() + sizeof(i32) +
               table_id_.size() + sizeof(i32) + index_name_.size() + sizeof(i32) + index_id_.size() + sizeof(SegmentID) + sizeof(i32);
    for (const auto &chunk_info : this->chunk_infos_) {
        size += chunk_info.GetSizeInBytes();
    }
    size += sizeof(i32) + sizeof(ChunkID) * this->deprecate_ids_.size();
    size += sizeof(i32) + table_key_.size();
    return size;
}

i32 WalCmdRenameTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
           this->new_table_name_.size();
}

i32 WalCmdRenameTableV2::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + db_name_.size() + sizeof(i32) + db_id_.size() + sizeof(i32) + table_name_.size() + sizeof(i32) +
           table_id_.size() + sizeof(i32) + new_table_name_.size() + sizeof(i32) + old_table_key_.size();
}

i32 WalCmdAddColumns::GetSizeInBytes() const {
    SizeT res = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32);
    for (const auto &column_def : column_defs_) {
        res += column_def->GetSizeInBytes();
    }
    return res;
}

i32 WalCmdAddColumnsV2::GetSizeInBytes() const {
    SizeT res = sizeof(WalCommandType) + sizeof(i32) + db_name_.size() + sizeof(i32) + db_id_.size() + sizeof(i32) + table_name_.size() +
                sizeof(i32) + table_id_.size() + sizeof(i32);
    for (const auto &column_def : column_defs_) {
        res += column_def->GetSizeInBytes();
    }
    res += sizeof(i32) + table_key_.size();
    return res;
}

i32 WalCmdDropColumns::GetSizeInBytes() const {
    SizeT res = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32);
    for (const auto &column_name : column_names_) {
        res += sizeof(i32) + column_name.size();
    }
    return res;
}

i32 WalCmdDropColumnsV2::GetSizeInBytes() const {
    SizeT res = sizeof(WalCommandType) + sizeof(i32) + db_name_.size() + sizeof(i32) + db_id_.size() + sizeof(i32) + table_name_.size() +
                sizeof(i32) + table_id_.size() + sizeof(i32);
    for (const auto &column_name : column_names_) {
        res += sizeof(i32) + column_name.size();
    }
    res += sizeof(i32) + column_ids_.size() * sizeof(ColumnID);
    res += sizeof(i32) + table_key_.size();
    res += sizeof(i32);
    for (const auto &column_key : column_keys_) {
        res += sizeof(i32) + column_key.size();
    }
    return res;
}

i32 WalCmdCleanup::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(timestamp_); }

void WalCmdCreateDatabase::WriteAdv(char *&buf) const {
    assert(!std::filesystem::path(db_dir_tail_).is_absolute());
    WriteBufAdv(buf, WalCommandType::CREATE_DATABASE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_dir_tail_);
    WriteBufAdv(buf, this->db_comment_);
}

void WalCmdCreateDatabaseV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_DATABASE_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->db_comment_);
}

void WalCmdDropDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_DATABASE);
    WriteBufAdv(buf, this->db_name_);
}

void WalCmdDropDatabaseV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_DATABASE_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->create_ts_);
}

void WalCmdCreateTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_TABLE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_dir_tail_);
    this->table_def_->WriteAdv(buf);
}

void WalCmdCreateTableV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_TABLE_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_id_);
    this->table_def_->WriteAdv(buf);
}

void WalCmdCreateIndex::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_INDEX);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_dir_tail_);
    index_base_->WriteAdv(buf);
    WriteBufAdv(buf, static_cast<i32>(segment_index_infos_.size()));
    for (const auto &segment_index_info : segment_index_infos_) {
        segment_index_info.WriteBufferAdv(buf);
    }
}

void WalCmdCreateIndexV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_INDEX_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, this->index_id_);
    index_base_->WriteAdv(buf);
    WriteBufAdv(buf, static_cast<i32>(segment_index_infos_.size()));
    for (const auto &segment_index_info : segment_index_infos_) {
        segment_index_info.WriteBufferAdv(buf);
    }
    WriteBufAdv(buf, this->table_key_);
}

void WalCmdDropTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_TABLE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
}

void WalCmdDropTableV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_TABLE_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, this->create_ts_);
    WriteBufAdv(buf, this->table_key_);
}

void WalCmdDropIndex::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_INDEX);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
}

void WalCmdDropIndexV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_INDEX_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->index_id_);
    WriteBufAdv(buf, this->create_ts_);
    WriteBufAdv(buf, this->index_key_);
}

void WalCmdImport::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::IMPORT);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    this->segment_info_.WriteBufferAdv(buf);
}

void WalCmdImportV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::IMPORT_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    this->segment_info_.WriteBufferAdv(buf);
}

void WalCmdAppend::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::APPEND);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    block_->WriteAdv(buf);
}

void WalCmdAppendV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::APPEND_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, static_cast<i32>(row_ranges_.size()));
    for (const auto &row_range : row_ranges_) {
        WriteBufAdv(buf, row_range.first);
        WriteBufAdv(buf, row_range.second);
    }
    block_->WriteAdv(buf);
}

void WalCmdDelete::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DELETE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, static_cast<i32>(this->row_ids_.size()));
    SizeT row_count = this->row_ids_.size();
    for (SizeT idx = 0; idx < row_count; ++idx) {
        const auto &row_id = this->row_ids_[idx];
        WriteBufAdv(buf, row_id);
    }
}

void WalCmdDeleteV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DELETE_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, static_cast<i32>(this->row_ids_.size()));
    SizeT row_count = this->row_ids_.size();
    for (SizeT idx = 0; idx < row_count; ++idx) {
        const auto &row_id = this->row_ids_[idx];
        WriteBufAdv(buf, row_id);
    }
}

void WalCmdSetSegmentStatusSealed::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::SET_SEGMENT_STATUS_SEALED);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_filter_binary_data_);
    i32 count = static_cast<i32>(this->block_filter_binary_data_.size());
    WriteBufAdv(buf, count);
    for (const auto &data : block_filter_binary_data_) {
        WriteBufAdv(buf, data.first);
        WriteBufAdv(buf, data.second);
    }
}

void WalCmdSetSegmentStatusSealedV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::SET_SEGMENT_STATUS_SEALED_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_filter_binary_data_);
    i32 count = static_cast<i32>(this->block_filter_binary_data_.size());
    WriteBufAdv(buf, count);
    for (const auto &data : block_filter_binary_data_) {
        WriteBufAdv(buf, data.first);
        WriteBufAdv(buf, data.second);
    }
}

void WalCmdUpdateSegmentBloomFilterData::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_filter_binary_data_);
    i32 count = static_cast<i32>(this->block_filter_binary_data_.size());
    WriteBufAdv(buf, count);
    for (const auto &data : block_filter_binary_data_) {
        WriteBufAdv(buf, data.first);
        WriteBufAdv(buf, data.second);
    }
}

void WalCmdUpdateSegmentBloomFilterDataV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, this->segment_filter_binary_data_);
    i32 count = static_cast<i32>(this->block_filter_binary_data_.size());
    WriteBufAdv(buf, count);
    for (const auto &data : block_filter_binary_data_) {
        WriteBufAdv(buf, data.first);
        WriteBufAdv(buf, data.second);
    }
}

void WalCmdCheckpoint::WriteAdv(char *&buf) const {
    assert(!std::filesystem::path(catalog_path_).is_absolute());
    WriteBufAdv(buf, WalCommandType::CHECKPOINT);
    WriteBufAdv(buf, this->max_commit_ts_);
    WriteBufAdv(buf, this->catalog_path_);
    WriteBufAdv(buf, this->catalog_name_);
}

void WalCmdCheckpointV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CHECKPOINT_V2);
    WriteBufAdv(buf, this->max_commit_ts_);
}

void WalCmdCompact::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::COMPACT);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    i32 new_segment_n = static_cast<i32>(this->new_segment_infos_.size());
    WriteBufAdv(buf, new_segment_n);
    for (i32 i = 0; i < new_segment_n; ++i) {
        this->new_segment_infos_[i].WriteBufferAdv(buf);
    }
    i32 deprecated_n = static_cast<i32>(this->deprecated_segment_ids_.size());
    WriteBufAdv(buf, deprecated_n);
    for (i32 i = 0; i < deprecated_n; ++i) {
        WriteBufAdv(buf, this->deprecated_segment_ids_[i]);
    }
}

void WalCmdCompactV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::COMPACT_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    i32 new_segment_n = static_cast<i32>(this->new_segment_infos_.size());
    WriteBufAdv(buf, new_segment_n);
    for (i32 i = 0; i < new_segment_n; ++i) {
        this->new_segment_infos_[i].WriteBufferAdv(buf);
    }
    i32 deprecated_n = static_cast<i32>(this->deprecated_segment_ids_.size());
    WriteBufAdv(buf, deprecated_n);
    for (i32 i = 0; i < deprecated_n; ++i) {
        WriteBufAdv(buf, this->deprecated_segment_ids_[i]);
    }
}

void WalCmdOptimize::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::OPTIMIZE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, static_cast<i32>(this->params_.size()));
    for (const auto &param : this->params_) {
        param->WriteAdv(buf);
    }
}

void WalCmdOptimizeV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::OPTIMIZE_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->index_id_);
    WriteBufAdv(buf, static_cast<i32>(this->params_.size()));
    for (const auto &param : this->params_) {
        param->WriteAdv(buf);
    }
}

void WalCmdDumpIndex::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DUMP_INDEX);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, static_cast<i32>(this->chunk_infos_.size()));
    for (const auto &chunk_info : this->chunk_infos_) {
        chunk_info.WriteBufferAdv(buf);
    }
    WriteBufAdv(buf, static_cast<i32>(this->deprecate_ids_.size()));
    for (const auto &chunk_id : this->deprecate_ids_) {
        WriteBufAdv(buf, chunk_id);
    }
}

void WalCmdDumpIndexV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DUMP_INDEX_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, this->index_name_);
    WriteBufAdv(buf, this->index_id_);
    WriteBufAdv(buf, this->segment_id_);
    WriteBufAdv(buf, static_cast<i32>(this->chunk_infos_.size()));
    for (const auto &chunk_info : this->chunk_infos_) {
        chunk_info.WriteBufferAdv(buf);
    }
    WriteBufAdv(buf, static_cast<i32>(this->deprecate_ids_.size()));
    for (const auto &chunk_id : this->deprecate_ids_) {
        WriteBufAdv(buf, chunk_id);
    }
    WriteBufAdv(buf, this->table_key_);
}

void WalCmdRenameTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::RENAME_TABLE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->new_table_name_);
}

void WalCmdRenameTableV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::RENAME_TABLE_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, this->new_table_name_);
    WriteBufAdv(buf, this->old_table_key_);
}

void WalCmdAddColumns::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::ADD_COLUMNS);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, static_cast<i32>(this->column_defs_.size()));
    for (const auto &column_def : column_defs_) {
        column_def->WriteAdv(buf);
    }
}

void WalCmdAddColumnsV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::ADD_COLUMNS_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, static_cast<i32>(this->column_defs_.size()));
    for (const auto &column_def : column_defs_) {
        column_def->WriteAdv(buf);
    }
    WriteBufAdv(buf, this->table_key_);
}

void WalCmdDropColumns::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_COLUMNS);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, static_cast<i32>(this->column_names_.size()));
    for (const auto &column_name : column_names_) {
        WriteBufAdv(buf, column_name);
    }
}

void WalCmdDropColumnsV2::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_COLUMNS_V2);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_id_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_id_);
    WriteBufAdv(buf, static_cast<i32>(this->column_names_.size()));
    for (const auto &column_name : column_names_) {
        WriteBufAdv(buf, column_name);
    }
    WriteBufAdv(buf, static_cast<i32>(this->column_ids_.size()));
    for (const auto &column_id : column_ids_) {
        WriteBufAdv(buf, column_id);
    }
    WriteBufAdv(buf, this->table_key_);
    WriteBufAdv(buf, static_cast<i32>(this->column_keys_.size()));
    for (const auto &column_key : column_keys_) {
        WriteBufAdv(buf, column_key);
    }
}

void WalCmdCleanup::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CLEANUP);
    WriteBufAdv(buf, this->timestamp_);
}

String WalCmdCreateDatabase::ToString() const {
    std::stringstream ss;
    ss << "Create Database: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db dir: " << db_dir_tail_ << std::endl;
    ss << "db comment: " << db_comment_ << std::endl;
    return std::move(ss).str();
}

String WalCmdCreateDatabaseV2::ToString() const {
    std::stringstream ss;
    ss << "Create Database: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "db comment: " << db_comment_ << std::endl;
    return std::move(ss).str();
}

String WalCmdDropDatabase::ToString() const {
    std::stringstream ss;
    ss << "Drop Database: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    return std::move(ss).str();
}

String WalCmdDropDatabaseV2::ToString() const {
    std::stringstream ss;
    ss << "Drop Database: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "create ts: " << create_ts_ << std::endl;
    return std::move(ss).str();
}

String WalCmdCreateTable::ToString() const {
    std::stringstream ss;
    ss << "Create Table: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_def_->ToString() << std::endl;
    ss << "table dir: " << table_dir_tail_ << std::endl;
    return std::move(ss).str();
}

String WalCmdCreateTableV2::ToString() const {
    std::stringstream ss;
    ss << "Create Table: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_def_->ToString() << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    return std::move(ss).str();
}

String WalCmdDropTable::ToString() const {
    std::stringstream ss;
    ss << "Drop Table: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    return std::move(ss).str();
}

String WalCmdDropTableV2::ToString() const {
    std::stringstream ss;
    ss << "Drop Table: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "create ts: " << create_ts_ << std::endl;
    ss << "table key: " << table_key_ << std::endl;
    return std::move(ss).str();
}

String WalCmdCreateIndex::ToString() const {
    std::stringstream ss;
    ss << "Create Index: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "index def: " << index_base_->ToString() << std::endl;
    for (const auto &segment_index_info : segment_index_infos_) {
        ss << segment_index_info.ToString() << std::endl;
    }
    return std::move(ss).str();
}

String WalCmdCreateIndexV2::ToString() const {
    std::stringstream ss;
    ss << "Create Index: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "index def: " << index_base_->ToString() << std::endl;
    for (const auto &segment_index_info : segment_index_infos_) {
        ss << segment_index_info.ToString() << std::endl;
    }
    ss << "table key: " << table_key_ << std::endl;
    return std::move(ss).str();
}

String WalCmdDropIndex::ToString() const {
    std::stringstream ss;
    ss << "Drop Index: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "index name: " << index_name_ << std::endl;
    return std::move(ss).str();
}

String WalCmdDropIndexV2::ToString() const {
    std::stringstream ss;
    ss << "Drop Index: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "index name: " << index_name_ << std::endl;
    ss << "index id: " << index_id_ << std::endl;
    ss << "create ts: " << create_ts_ << std::endl;
    ss << "index key: " << index_key_ << std::endl;
    return std::move(ss).str();
}

String WalCmdImport::ToString() const {
    std::stringstream ss;
    ss << "Import: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    auto &segment_info = segment_info_;
    ss << segment_info.ToString() << std::endl;
    return std::move(ss).str();
}

String WalCmdImportV2::ToString() const {
    std::stringstream ss;
    ss << "Import: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    auto &segment_info = segment_info_;
    ss << segment_info.ToString() << std::endl;
    return std::move(ss).str();
}

String WalCmdAppend::ToString() const {
    std::stringstream ss;
    ss << "Append: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << block_->ToBriefString();
    return std::move(ss).str();
}

String WalCmdAppendV2::ToString() const {
    std::stringstream ss;
    ss << "Append: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "row range: ";
    for (auto &row_range : row_ranges_) {
        ss << "[" << row_range.first.ToString() << ", +" << row_range.second << "] | ";
    }
    ss << block_->ToBriefString();
    return std::move(ss).str();
}

String WalCmdDelete::ToString() const {
    std::stringstream ss;
    ss << "Delete: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "delete row cout: " << row_ids_.size() << std::endl;
    return std::move(ss).str();
}

String WalCmdDeleteV2::ToString() const {
    return fmt::format("Delete: db name: {}, db id: {}, table name: {}, table id: {}, delete row count: {}",
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ids_.size());
}

String WalCmdSetSegmentStatusSealed::ToString() const {
    std::stringstream ss;
    ss << "Set Segment Status Sealed: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "segment id: " << segment_id_ << std::endl;
    return std::move(ss).str();
}

String WalCmdSetSegmentStatusSealedV2::ToString() const {
    std::stringstream ss;
    ss << "Set Segment Status Sealed: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "segment id: " << segment_id_ << std::endl;
    return std::move(ss).str();
}

String WalCmdUpdateSegmentBloomFilterData::ToString() const {
    std::stringstream ss;
    ss << "Update Segment Bloom Filter Data: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "segment id: " << segment_id_ << std::endl;
    return std::move(ss).str();
}

String WalCmdUpdateSegmentBloomFilterDataV2::ToString() const {
    std::stringstream ss;
    ss << "Update Segment Bloom Filter Data: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "segment id: " << segment_id_ << std::endl;
    return std::move(ss).str();
}

String WalCmdCheckpoint::ToString() const {
    std::stringstream ss;
    ss << "Checkpoint: " << std::endl;
    ss << "catalog path: " << fmt::format("{}/{}", catalog_path_, catalog_name_) << std::endl;
    ss << "max commit ts: " << max_commit_ts_ << std::endl;
    return std::move(ss).str();
}

String WalCmdCheckpointV2::ToString() const {
    std::stringstream ss;
    ss << "Checkpoint: " << std::endl;
    ss << "max commit ts: " << max_commit_ts_ << std::endl;
    return std::move(ss).str();
}

String WalCmdCompact::ToString() const {
    std::stringstream ss;
    ss << "Compact: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "deprecated segment: ";
    for (SegmentID segment_id : deprecated_segment_ids_) {
        ss << segment_id << " | ";
    }
    ss << std::endl;
    ss << "new segment: ";
    for (auto &new_seg_info : new_segment_infos_) {
        ss << new_seg_info.ToString() << " | ";
    }
    ss << std::endl;
    return std::move(ss).str();
}

String WalCmdCompactV2::ToString() const {
    std::stringstream ss;
    ss << "Compact: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "deprecated segment: ";
    for (SegmentID segment_id : deprecated_segment_ids_) {
        ss << segment_id << " | ";
    }
    ss << std::endl;
    ss << "new segment: ";
    for (auto &new_seg_info : new_segment_infos_) {
        ss << new_seg_info.ToString() << " | ";
    }
    ss << std::endl;
    return std::move(ss).str();
}

String WalCmdOptimize::ToString() const {
    std::stringstream ss;
    ss << "Optimize: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "index name: " << index_name_ << std::endl;
    ss << "index parameter: ";
    for (auto &param_ptr : params_) {
        ss << param_ptr->ToString() << " | ";
    }
    ss << std::endl;
    return std::move(ss).str();
}

String WalCmdOptimizeV2::ToString() const {
    std::stringstream ss;
    ss << "Optimize: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "index name: " << index_name_ << std::endl;
    ss << "index id: " << index_id_ << std::endl;
    ss << "index parameter: ";
    for (auto &param_ptr : params_) {
        ss << param_ptr->ToString() << " | ";
    }
    ss << std::endl;
    return std::move(ss).str();
}

String WalCmdDumpIndex::ToString() const {
    std::stringstream ss;
    ss << "Dump Index: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "index name: " << index_name_ << std::endl;
    ss << "segment id: " << segment_id_ << std::endl;
    ss << "chunk infos: ";
    for (auto &chunk_info : chunk_infos_) {
        ss << chunk_info.ToString() << " | ";
    }
    ss << "deprecated chunk ids: ";
    for (auto &chunk_id : deprecate_ids_) {
        ss << chunk_id << " | ";
    }
    ss << std::endl;
    return std::move(ss).str();
}

String WalCmdDumpIndexV2::ToString() const {
    std::stringstream ss;
    ss << "Dump Index: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "index name: " << index_name_ << std::endl;
    ss << "index id: " << index_id_ << std::endl;
    ss << "segment id: " << segment_id_ << std::endl;
    ss << "chunk infos: ";
    for (auto &chunk_info : chunk_infos_) {
        ss << chunk_info.ToString() << " | ";
    }
    ss << std::endl;
    ss << "deprecated chunk ids: ";
    for (auto &chunk_id : deprecate_ids_) {
        ss << chunk_id << " | ";
    }
    ss << std::endl;
    ss << "table key: " << table_key_ << std::endl;
    return std::move(ss).str();
}

String WalCmdRenameTable::ToString() const {
    std::stringstream ss;
    ss << "Rename Table: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "new table name: " << new_table_name_ << std::endl;
    return std::move(ss).str();
}

String WalCmdRenameTableV2::ToString() const {
    std::stringstream ss;
    ss << "Rename Table: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "new table name: " << new_table_name_ << std::endl;
    ss << "old table key: " << old_table_key_ << std::endl;
    return std::move(ss).str();
}

String WalCmdAddColumns::ToString() const {
    std::stringstream ss;
    ss << "Add Columns: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "columns: ";
    for (auto &column_def : column_defs_) {
        ss << column_def->ToString() << " | ";
    }
    return std::move(ss).str();
}

String WalCmdAddColumnsV2::ToString() const {
    std::stringstream ss;
    ss << "Add Columns: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "columns: ";
    for (auto &column_def : column_defs_) {
        ss << column_def->ToString() << " | ";
    }
    ss << std::endl;
    ss << "table key: " << table_key_ << std::endl;
    return std::move(ss).str();
}

String WalCmdDropColumns::ToString() const {
    std::stringstream ss;
    ss << "Drop Columns: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "columns: ";
    for (auto &column_name : column_names_) {
        ss << column_name << " | ";
    }
    return std::move(ss).str();
}

String WalCmdDropColumnsV2::ToString() const {
    std::stringstream ss;
    ss << "Drop Columns: " << std::endl;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db id: " << db_id_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "table id: " << table_id_ << std::endl;
    ss << "columns: ";
    for (auto &column_name : column_names_) {
        ss << column_name << " | ";
    }
    ss << std::endl;
    ss << "table key: " << table_key_ << std::endl;
    ss << "column keys: ";
    for (auto &column_key : column_keys_) {
        ss << column_key << " | ";
    }
    ss << std::endl;
    return std::move(ss).str();
}

String WalCmdCleanup::ToString() const {
    std::stringstream ss;
    ss << "Cleanup: " << std::endl;
    ss << "timestamp: " << timestamp_ << std::endl;
    return std::move(ss).str();
}

String WalCmdCreateDatabase::CompactInfo() const {
    return fmt::format("{}: database: {}, dir: {}, comment: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, db_dir_tail_, db_comment_);
}

String WalCmdCreateDatabaseV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id:{}, comment: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, db_id_, db_comment_);
}

String WalCmdDropDatabase::CompactInfo() const { return fmt::format("{}: database: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_); }

String WalCmdDropDatabaseV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, create_ts: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, db_id_, create_ts_);
}

String WalCmdCreateTable::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, dir: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_def_->ToString(),
                       table_dir_tail_);
}

String WalCmdCreateTableV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table_id: {}, table: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_id_,
                       table_def_->ToString());
}

String WalCmdDropTable::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_);
}

String WalCmdDropTableV2::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, table_id: {}, create_ts: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       table_id_,
                       create_ts_);
}

String WalCmdCreateIndex::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, index: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       index_base_->ToString());
}

String WalCmdCreateIndexV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index_id: {}, index: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       index_id_,
                       index_base_->ToString());
}

String WalCmdDropIndex::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, index: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_, index_name_);
}

String WalCmdDropIndexV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}, create_ts_: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       index_name_,
                       index_id_,
                       create_ts_);
}

String WalCmdImport::CompactInfo() const {
    auto &segment_info = segment_info_;
    return fmt::format("{}: database: {}, table: {}, segment: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       segment_info.ToString());
}

String WalCmdImportV2::CompactInfo() const {
    auto &segment_info = segment_info_;
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, segment: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       segment_info.ToString());
}

String WalCmdAppend::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, block: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       block_->ToBriefString());
}

String WalCmdAppendV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, block: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       block_->ToBriefString());
}

String WalCmdDelete::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, deleted: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_, row_ids_.size());
}

String WalCmdDeleteV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, deleted: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       row_ids_.size());
}

String WalCmdSetSegmentStatusSealed::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, segment: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_, segment_id_);
}

String WalCmdSetSegmentStatusSealedV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, segment: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       segment_id_);
}

String WalCmdUpdateSegmentBloomFilterData::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, segment: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_, segment_id_);
}

String WalCmdUpdateSegmentBloomFilterDataV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, segment: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       segment_id_);
}

String WalCmdCheckpoint::CompactInfo() const {
    return fmt::format("{}: path: {}, max_commit_ts: {},",
                       WalCmd::WalCommandTypeToString(GetType()),
                       fmt::format("{}/{}", catalog_path_, catalog_name_),
                       max_commit_ts_);
}

String WalCmdCheckpointV2::CompactInfo() const {
    return fmt::format("{}: max_commit_ts: {}", WalCmd::WalCommandTypeToString(GetType()), max_commit_ts_);
}

String WalCmdCompact::CompactInfo() const {
    std::stringstream ss;
    ss << WalCmd::WalCommandTypeToString(GetType()) << ": ";
    ss << "database: " << db_name_;
    ss << "table: " << table_name_ << std::endl;
    ss << "deprecated segment: ";
    for (SegmentID segment_id : deprecated_segment_ids_) {
        ss << segment_id << " | ";
    }
    ss << std::endl;
    ss << "new segment: ";
    for (auto &new_seg_info : new_segment_infos_) {
        ss << new_seg_info.ToString() << " | ";
    }
    ss << std::endl;
    return String();
}

String WalCmdCompactV2::CompactInfo() const {
    std::stringstream ss;
    ss << WalCmd::WalCommandTypeToString(GetType()) << ": ";
    ss << "database: " << db_name_;
    ss << "db_id: " << db_id_;
    ss << "table: " << table_name_ << std::endl;
    ss << "table_id: " << table_id_ << std::endl;
    ss << "deprecated segment: ";
    for (SegmentID segment_id : deprecated_segment_ids_) {
        ss << segment_id << " | ";
    }
    ss << std::endl;
    ss << "new segment: ";
    for (auto &new_seg_info : new_segment_infos_) {
        ss << new_seg_info.ToString() << " | ";
    }
    ss << std::endl;
    return String();
}

String WalCmdOptimize::CompactInfo() const {
    std::stringstream ss;
    for (auto &param_ptr : params_) {
        ss << param_ptr->ToString() << " | ";
    }
    return fmt::format("{}: database: {}, table: {}, index: {}, parameter: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       index_name_,
                       ss.str());
}

String WalCmdOptimizeV2::CompactInfo() const {
    std::stringstream ss;
    for (auto &param_ptr : params_) {
        ss << param_ptr->ToString() << " | ";
    }
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}, parameter: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       index_name_,
                       index_id_,
                       ss.str());
}

String WalCmdDumpIndex::CompactInfo() const {
    std::stringstream ss;
    for (auto &chunk_info : chunk_infos_) {
        ss << chunk_info.ToString() << " | ";
    }

    return fmt::format("{}: database: {}, table: {}, index: {}, segment: {}, parameter: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       index_name_,
                       segment_id_,
                       ss.str());

    return std::move(ss).str();
}

String WalCmdDumpIndexV2::CompactInfo() const {
    std::stringstream ss;
    for (auto &chunk_info : chunk_infos_) {
        ss << chunk_info.ToString() << " | ";
    }

    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, index: {}, index_id: {}, segment: {}, parameter: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       index_name_,
                       index_id_,
                       segment_id_,
                       ss.str());

    return std::move(ss).str();
}

String WalCmdRenameTable::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, new table: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       new_table_name_);
}

String WalCmdRenameTableV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, old_talbe_key: {}, new table: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       old_table_key_,
                       new_table_name_);
}

String WalCmdAddColumns::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, columns: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       column_defs_.size());
}

String WalCmdAddColumnsV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       column_defs_.size());
}

String WalCmdDropColumns::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, columns: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       column_names_.size());
}

String WalCmdDropColumnsV2::CompactInfo() const {
    return fmt::format("{}: database: {}, db_id: {}, table: {}, table_id: {}, columns: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       db_id_,
                       table_name_,
                       table_id_,
                       column_names_.size());
}

String WalCmdCleanup::CompactInfo() const { return fmt::format("{}: timestamp: {}", WalCmd::WalCommandTypeToString(GetType()), timestamp_); }

bool WalEntry::operator==(const WalEntry &other) const {
    if (this->txn_id_ != other.txn_id_ || this->commit_ts_ != other.commit_ts_ || this->cmds_.size() != other.cmds_.size()) {
        return false;
    }
    for (u32 i = 0; i < this->cmds_.size(); i++) {
        const SharedPtr<WalCmd> &cmd1 = this->cmds_[i];
        const SharedPtr<WalCmd> &cmd2 = other.cmds_[i];
        if (cmd1.get() == nullptr || cmd2.get() == nullptr || (*cmd1).operator!=(*cmd2)) {
            return false;
        }
    }
    return true;
}

bool WalEntry::operator!=(const WalEntry &other) const { return !operator==(other); }

i32 WalEntry::GetSizeInBytes() const {
    i32 size = sizeof(WalEntryHeader) + sizeof(i32);
    SizeT cmd_count = cmds_.size();
    for (SizeT idx = 0; idx < cmd_count; ++idx) {
        const auto &cmd = cmds_[idx];
        size += cmd->GetSizeInBytes();
    }
    size += sizeof(i32); // pad
    return size;
}

/**
 * An entry is serialized as follows:
 * - WalEntryHeader
 *   - size
 *   - checksum
 *   - txn_id
 *   - commit_ts
 * - number of WalCmd
 *   - (repeated) WalCmd
 * - 4 bytes pad
 * @param ptr
 */

void WalEntry::WriteAdv(char *&ptr) const {
    char *const saved_ptr = ptr;
    std::memcpy(ptr, this, sizeof(WalEntryHeader));
    ptr += sizeof(WalEntryHeader);
    WriteBufAdv(ptr, static_cast<i32>(cmds_.size()));
    SizeT cmd_count = cmds_.size();
    for (SizeT idx = 0; idx < cmd_count; ++idx) {
        const auto &cmd = cmds_[idx];
        cmd->WriteAdv(ptr);
    }
    i32 size = ptr - saved_ptr + sizeof(i32);
    WriteBufAdv(ptr, size);
    auto *header = (WalEntryHeader *)saved_ptr;
    header->size_ = size;
    header->checksum_ = 0;
    // CRC32IEEE is equivalent to boost::crc_32_type on
    // little-endian machine.
    header->checksum_ = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(saved_ptr), size);
}

SharedPtr<WalEntry> WalEntry::ReadAdv(const char *&ptr, i32 max_bytes) {
    const char *const ptr_end = ptr + max_bytes;
    if (max_bytes <= 0) {
        String error_message = "ptr goes out of range when reading WalEntry";
        LOG_WARN(error_message);
        return nullptr;
    }
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    auto *header = reinterpret_cast<WalEntryHeader *>(const_cast<char *>(ptr));
    entry->size_ = header->size_;
    entry->checksum_ = header->checksum_;
    entry->txn_id_ = header->txn_id_;
    entry->commit_ts_ = header->commit_ts_;
    if (const i32 size2 = ReadBuf<i32>(ptr + entry->size_ - sizeof(i32)); entry->size_ != size2) {
        return nullptr;
    }
    {
        header->checksum_ = 0;
        DeferFn defer([&] { header->checksum_ = entry->checksum_; });
        if (const u32 checksum2 = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(ptr), entry->size_); entry->checksum_ != checksum2) {
            return nullptr;
        }
    }
    ptr += sizeof(WalEntryHeader);
    i32 cnt = ReadBufAdv<i32>(ptr);
    for (i32 i = 0; i < cnt; i++) {
        max_bytes = ptr_end - ptr;
        if (max_bytes <= 0) {
            String error_message = "ptr goes out of range when reading WalEntry";
            LOG_WARN(error_message);
            return nullptr;
        }
        SharedPtr<WalCmd> cmd = WalCmd::ReadAdv(ptr, max_bytes);
        entry->cmds_.push_back(cmd);
    }
    ptr += sizeof(i32);
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        String error_message = "ptr goes out of range when reading WalEntry";
        LOG_WARN(error_message);
        return nullptr;
    }
    return entry;
}

bool WalEntry::IsCheckPoint(WalCmdCheckpoint *&last_checkpoint_cmd) const {
    TxnTimeStamp max_commit_ts = 0;
    bool found = false;
    for (auto &cmd : cmds_) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            auto checkpoint_cmd = static_cast<WalCmdCheckpoint *>(cmd.get());
            if (!found || TxnTimeStamp(checkpoint_cmd->max_commit_ts_) > max_commit_ts) {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                last_checkpoint_cmd = checkpoint_cmd;
                found = true;
            }
        }
    }
    return found;
}

bool WalEntry::IsCheckPoint(WalCmdCheckpointV2 *&last_checkpoint_cmd) const {
    TxnTimeStamp max_commit_ts = 0;
    bool found = false;
    for (auto &cmd : cmds_) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT_V2) {
            auto checkpoint_cmd = static_cast<WalCmdCheckpointV2 *>(cmd.get());
            if (!found || TxnTimeStamp(checkpoint_cmd->max_commit_ts_) > max_commit_ts) {
                max_commit_ts = checkpoint_cmd->max_commit_ts_;
                last_checkpoint_cmd = checkpoint_cmd;
                found = true;
            }
        }
    }
    return found;
}

String WalEntry::ToString() const {
    std::stringstream ss;
    ss << "\n======= WAL ENTRY =======" << std::endl;
    ss << "[HEADER]" << std::endl;
    ss << "txn id: " << txn_id_ << std::endl;
    ss << "commit ts: " << commit_ts_ << std::endl;
    ss << "size: " << size_ << std::endl;
    for (const auto &cmd : cmds_) {
        ss << "[" << WalCmd::WalCommandTypeToString(cmd->GetType()) << "]" << std::endl;
        ss << cmd->ToString();
    }
    ss << "========================" << std::endl;
    return std::move(ss).str();
}

String WalEntry::CompactInfo() const {
    std::stringstream ss;
    SizeT cmd_size = cmds_.size();
    for (SizeT idx = 0; idx < cmd_size - 1; ++idx) {
        auto &cmd = cmds_[idx];
        ss << cmd->CompactInfo() << std::endl;
    }
    if (cmds_.size() > 0) {
        ss << cmds_.back()->CompactInfo();
    }
    return std::move(ss).str();
}

String WalCmd::WalCommandTypeToString(WalCommandType type) {
    String command{};
    switch (type) {
        case WalCommandType::INVALID:
            command = "INVALID";
            break;
        case WalCommandType::CREATE_DATABASE:
            command = "CREATE_DATABASE";
            break;
        case WalCommandType::CREATE_DATABASE_V2:
            command = "CREATE_DATABASE_V2";
            break;
        case WalCommandType::DROP_DATABASE:
            command = "DROP_DATABASE";
            break;
        case WalCommandType::DROP_DATABASE_V2:
            command = "DROP_DATABASE_V2";
            break;
        case WalCommandType::CREATE_TABLE:
            command = "CREATE_TABLE";
            break;
        case WalCommandType::CREATE_TABLE_V2:
            command = "CREATE_TABLE_V2";
            break;
        case WalCommandType::DROP_TABLE:
            command = "DROP_TABLE";
            break;
        case WalCommandType::DROP_TABLE_V2:
            command = "DROP_TABLE_V2";
            break;
        case WalCommandType::ALTER_INFO:
            command = "ALTER_INFO";
            break;
        case WalCommandType::ALTER_INFO_V2:
            command = "ALTER_INFO_V2";
            break;
        case WalCommandType::IMPORT:
            command = "IMPORT";
            break;
        case WalCommandType::IMPORT_V2:
            command = "IMPORT_V2";
            break;
        case WalCommandType::APPEND:
            command = "APPEND";
            break;
        case WalCommandType::APPEND_V2:
            command = "APPEND_V2";
            break;
        case WalCommandType::DELETE:
            command = "DELETE";
            break;
        case WalCommandType::DELETE_V2:
            command = "DELETE_V2";
            break;
        case WalCommandType::SET_SEGMENT_STATUS_SEALED:
            command = "SET_SEGMENT_STATUS_SEALED";
            break;
        case WalCommandType::SET_SEGMENT_STATUS_SEALED_V2:
            command = "SET_SEGMENT_STATUS_SEALED_V2";
            break;
        case WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA:
            command = "UPDATE_SEGMENT_BLOOM_FILTER_DATA";
            break;
        case WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA_V2:
            command = "UPDATE_SEGMENT_BLOOM_FILTER_DATA_V2";
            break;
        case WalCommandType::CHECKPOINT:
            command = "CHECKPOINT";
            break;
        case WalCommandType::CHECKPOINT_V2:
            command = "CHECKPOINT_V2";
            break;
        case WalCommandType::CREATE_INDEX:
            command = "CREATE_INDEX";
            break;
        case WalCommandType::CREATE_INDEX_V2:
            command = "CREATE_INDEX_V2";
            break;
        case WalCommandType::DROP_INDEX:
            command = "DROP_INDEX";
            break;
        case WalCommandType::DROP_INDEX_V2:
            command = "DROP_INDEX_V2";
            break;
        case WalCommandType::COMPACT:
            command = "COMPACT";
            break;
        case WalCommandType::COMPACT_V2:
            command = "COMPACT_V2";
            break;
        case WalCommandType::OPTIMIZE:
            command = "OPTIMIZE";
            break;
        case WalCommandType::OPTIMIZE_V2:
            command = "OPTIMIZE_V2";
            break;
        case WalCommandType::DUMP_INDEX:
            command = "DUMP_INDEX";
            break;
        case WalCommandType::DUMP_INDEX_V2:
            command = "DUMP_INDEX_V2";
            break;
        case WalCommandType::RENAME_TABLE:
            command = "RENAME_TABLE";
            break;
        case WalCommandType::RENAME_TABLE_V2:
            command = "RENAME_TABLE_V2";
            break;
        case WalCommandType::ADD_COLUMNS:
            command = "ADD_COLUMNS";
            break;
        case WalCommandType::ADD_COLUMNS_V2:
            command = "ADD_COLUMNS_V2";
            break;
        case WalCommandType::DROP_COLUMNS:
            command = "DROP_COLUMNS";
            break;
        case WalCommandType::DROP_COLUMNS_V2:
            command = "DROP_COLUMNS_V2";
            break;
        case WalCommandType::CLEANUP:
            command = "CLEAN_UP";
            break;
        default: {
            String error_message = "Unknown command type";
            UnrecoverableError(error_message);
        }
    }
    return command;
}

UniquePtr<WalEntryIterator> WalEntryIterator::Make(const String &wal_path, bool is_backward) {
    std::ifstream ifs(wal_path.c_str(), std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        String error_message = "WAL open failed";
        UnrecoverableError(error_message);
    }
    auto wal_size = ifs.tellg();
    Vector<char> buf(wal_size);
    ifs.seekg(0, std::ios::beg);
    ifs.read(buf.data(), wal_size);
    ifs.close();

    return MakeUnique<WalEntryIterator>(std::move(buf), wal_size, is_backward);
}

SharedPtr<WalEntry> WalEntryIterator::Next() {
    if (is_backward_) {
        assert(off_ > 0);
        const i32 entry_size = ReadBuf<i32>(buf_.data() + off_ - sizeof(i32));
        if ((SizeT)entry_size > off_) {
            return nullptr;
        }
        const char *ptr = buf_.data() + off_ - (SizeT)entry_size;
        auto entry = WalEntry::ReadAdv(ptr, entry_size);
        if (entry.get() != nullptr) {
            off_ -= entry_size;
        }
        return entry;
    } else {
        assert(off_ < buf_.size());
        const i32 entry_size = ReadBuf<i32>(buf_.data() + off_);
        if (off_ + (SizeT)entry_size > buf_.size()) {
            return nullptr;
        }
        const char *ptr = buf_.data() + off_;
        auto entry = WalEntry::ReadAdv(ptr, entry_size);
        if (entry.get() != nullptr) {
            off_ += (SizeT)entry_size;
        }
        return entry;
    }
}

SharedPtr<WalEntry> WalEntryIterator::GetEntryByIndex(i64 index) {
    i64 count = 0;
    while (HasNext()) {
        if (count == index) {
            return Next();
        } else {
            Next();
        }
        ++count;
    }
    return nullptr;
}

Vector<SharedPtr<WalEntry>> WalEntryIterator::GetAllEntries() {
    Vector<SharedPtr<WalEntry>> entries;
    while (HasNext()) {
        entries.emplace_back(Next());
    }
    if (is_backward_) {
        std::reverse(entries.begin(), entries.end());
    }
    return entries;
}

bool WalEntryIterator::IsGood() const { return (is_backward_ && off_ == 0) || (!is_backward_ && off_ == buf_.size()); }

WalListIterator::WalListIterator(const Vector<String> &wal_list) {
    assert(!wal_list.empty());
    for (SizeT i = 0; i < wal_list.size(); ++i) {
        wal_list_.push_back(wal_list[i]);
    }
    PurgeBadEntriesAfterLatestCheckpoint();
    if (!wal_list_.empty())
        iter_ = WalEntryIterator::Make(wal_list_.front(), true);
}

void WalListIterator::PurgeBadEntriesAfterLatestCheckpoint() {
    bool found_checkpoint = false;
    SizeT file_num = 0;
    auto it = wal_list_.begin();
    while (!found_checkpoint && it != wal_list_.end()) {
        i64 bad_offset = i64(-1);
        iter_ = WalEntryIterator::Make(*it, false);
        while (iter_->HasNext()) {
            auto entry = iter_->Next();
            if (entry.get() != nullptr) {
                {
                    WalCmdCheckpointV2 *checkpoint_cmd = nullptr;
                    if (entry->IsCheckPoint(checkpoint_cmd)) {
                        found_checkpoint = true;
                    }
                }
                {
                    WalCmdCheckpoint *checkpoint_cmd = nullptr;
                    if (entry->IsCheckPoint(checkpoint_cmd)) {
                        found_checkpoint = true;
                    }
                }
            } else {
                bad_offset = iter_->GetOffset();
                break;
            }
        }
        if (bad_offset != i64(-1)) {
            String error_message = fmt::format("Found bad wal entry {}@{}", *it, bad_offset);
            LOG_WARN(error_message);
            if (bad_offset == 0) {
                error_message = fmt::format("Remove wal log {}", *it);
                LOG_WARN(error_message);
                VirtualStore::DeleteFile(*it);
                ++it;
                for (SizeT i = 0; i <= file_num; ++i) {
                    wal_list_.pop_front();
                }
                file_num = 0;
            } else {
                VirtualStore::Truncate(*it, bad_offset);
                error_message = fmt::format("Truncated {}@{}", *it, bad_offset);
                LOG_WARN(error_message);
                ++it;
                for (SizeT i = 0; i < file_num; ++i) {
                    wal_list_.pop_front();
                }
                file_num = 1;
            }
        } else {
            ++it;
            ++file_num;
        }
    }
    iter_.reset();
}

bool WalListIterator::HasNext() {
    if (iter_.get() == nullptr) {
        return false;
    }
    if (iter_->HasNext()) {
        return true;
    }
    while (1) {
        if (wal_list_.size() <= 1) {
            return false;
        }
        wal_list_.pop_front();
        iter_ = WalEntryIterator::Make(wal_list_.front(), true);
        if (iter_->HasNext()) {
            return true;
        }
    }
}

SharedPtr<WalEntry> WalListIterator::Next() {
    auto entry = iter_->Next();
    if (entry.get() == nullptr) {
        auto off = iter_->GetOffset();
        String error_message = fmt::format("Found bad wal entry {}@{}", wal_list_.front(), off);
        LOG_WARN(error_message);
    }
    return entry;
}

} // namespace infinity

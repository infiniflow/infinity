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
#include <fstream>
#include <vector>

module wal_entry;

import crc;
import serialize;
import data_block;
import table_def;
import index_base;
import infinity_exception;

import stl;

import third_party;
import internal_types;
import logger;
import block_entry;
import segment_entry;
import segment_index_entry;
import chunk_index_entry;
import block_version;
import local_file_system;
import index_defines;
import create_index_info;
import persistence_manager;
import infinity_context;

namespace infinity {

WalBlockInfo::WalBlockInfo(BlockEntry *block_entry)
    : block_id_(block_entry->block_id_), row_count_(block_entry->block_row_count_), row_capacity_(block_entry->row_capacity_) {
    outline_infos_.resize(block_entry->columns_.size());
    for (SizeT i = 0; i < block_entry->columns_.size(); i++) {
        auto &col_i_outline_info = outline_infos_[i];
        auto *column = block_entry->columns_[i].get();
        col_i_outline_info = {column->OutlineBufferCount(0), column->LastChunkOff(0)};
        Vector<String> paths = column->FilePaths();
        paths_.insert(paths_.end(), paths.begin(), paths.end());
    }
    String file_dir = fmt::format("{}/{}", *block_entry->base_dir(), *block_entry->block_dir());
    String version_file_path = fmt::format("{}/{}", file_dir, *BlockVersion::FileName());
    paths_.push_back(version_file_path);
    auto *pm = InfinityContext::instance().persistence_manager();
    addr_serializer_.Initialize(pm, paths_);
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

WalBlockInfo WalBlockInfo::ReadBufferAdv(char *&ptr) {
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
    return ss.str();
}

WalSegmentInfo::WalSegmentInfo(SegmentEntry *segment_entry)
    : segment_id_(segment_entry->segment_id_), column_count_(segment_entry->column_count_), row_count_(segment_entry->row_count_),
      actual_row_count_(segment_entry->actual_row_count_), row_capacity_(segment_entry->row_capacity_) {
    for (auto &block_entry : segment_entry->block_entries_) {
        block_infos_.push_back(WalBlockInfo(block_entry.get()));
    }
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

WalSegmentInfo WalSegmentInfo::ReadBufferAdv(char *&ptr) {
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
    return ss.str();
}

WalChunkIndexInfo::WalChunkIndexInfo(ChunkIndexEntry *chunk_index_entry)
    : chunk_id_(chunk_index_entry->chunk_id_), base_name_(chunk_index_entry->base_name_), base_rowid_(chunk_index_entry->base_rowid_),
      row_count_(chunk_index_entry->row_count_), deprecate_ts_(chunk_index_entry->deprecate_ts_) {
    SegmentIndexEntry *segment_index_entry = chunk_index_entry->segment_index_entry_;
    IndexType index_type = segment_index_entry->table_index_entry()->index_base()->index_type_;
    switch (index_type) {
        case IndexType::kFullText: {
            String full_path = fmt::format("{}/{}", *chunk_index_entry->base_dir_, *(segment_index_entry->index_dir()));
            paths_.push_back(full_path + chunk_index_entry->base_name_ + POSTING_SUFFIX);
            paths_.push_back(full_path + chunk_index_entry->base_name_ + DICT_SUFFIX);
            paths_.push_back(full_path + chunk_index_entry->base_name_ + LENGTH_SUFFIX);
            break;
        }
        case IndexType::kHnsw:
        case IndexType::kEMVB:
        case IndexType::kSecondary:
        case IndexType::kBMP: {
            String full_dir = fmt::format("{}/{}", *chunk_index_entry->base_dir_, *(segment_index_entry->index_dir()));
            String file_name = ChunkIndexEntry::IndexFileName(segment_index_entry->segment_id(), chunk_index_entry->chunk_id_);
            String full_path = fmt::format("{}/{}", full_dir, file_name);
            paths_.push_back(full_path);
            break;
        }
        default: {
            String error_message = "Unsupported index type when add wal.";
            UnrecoverableError(error_message);
        }
    }
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
        pm_size_= addr_serializer_.GetSizeInBytes();
        size += pm_size_;
    }
    return size + sizeof(ChunkID) + sizeof(i32) + base_name_.size() + sizeof(base_rowid_) + sizeof(row_count_) + sizeof(deprecate_ts_);
}

void WalChunkIndexInfo::WriteBufferAdv(char *&buf) const {
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

WalChunkIndexInfo WalChunkIndexInfo::ReadBufferAdv(char *&ptr) {
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
    return ss.str();
}

SharedPtr<WalCmd> WalCmd::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_end = ptr + max_bytes;
    SharedPtr<WalCmd> cmd = nullptr;
    auto cmd_type = ReadBufAdv<WalCommandType>(ptr);
    switch (cmd_type) {
        case WalCommandType::CREATE_DATABASE: {
            String db_name = ReadBufAdv<String>(ptr);
            String db_dir_tail = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdCreateDatabase>(std::move(db_name), std::move(db_dir_tail));
            break;
        }
        case WalCommandType::DROP_DATABASE: {
            String db_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropDatabase>(std::move(db_name));
            break;
        }
        case WalCommandType::CREATE_TABLE: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_dir_tail = ReadBufAdv<String>(ptr);
            SharedPtr<TableDef> table_def = TableDef::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdCreateTable>(std::move(db_name), std::move(table_dir_tail), table_def);
            break;
        }
        case WalCommandType::DROP_TABLE: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropTable>(db_name, table_name);
            break;
        }
        case WalCommandType::IMPORT: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            WalSegmentInfo segment_info = WalSegmentInfo::ReadBufferAdv(ptr);
            cmd = MakeShared<WalCmdImport>(db_name, table_name, std::move(segment_info));
            break;
        }
        case WalCommandType::APPEND: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            SharedPtr<DataBlock> block = DataBlock::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdAppend>(db_name, table_name, block);
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
            cmd = MakeShared<WalCmdSetSegmentStatusSealed>(std::move(db_name),
                                                           std::move(table_name),
                                                           segment_id,
                                                           std::move(segment_filter_binary_data),
                                                           std::move(block_filter_binary_data));
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
            cmd = MakeShared<WalCmdUpdateSegmentBloomFilterData>(std::move(db_name),
                                                                 std::move(table_name),
                                                                 segment_id,
                                                                 std::move(segment_filter_binary_data),
                                                                 std::move(block_filter_binary_data));
            break;
        }
        case WalCommandType::CHECKPOINT: {
            i64 max_commit_ts = ReadBufAdv<i64>(ptr);
            bool is_full_checkpoint = ReadBufAdv<i8>(ptr);
            String catalog_path = ReadBufAdv<String>(ptr);
            String catalog_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdCheckpoint>(max_commit_ts, is_full_checkpoint, catalog_path, catalog_name);
            break;
        }
        case WalCommandType::CREATE_INDEX: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_dir_tail = ReadBufAdv<String>(ptr);
            SharedPtr<IndexBase> index_base = IndexBase::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdCreateIndex>(std::move(db_name), std::move(table_name), std::move(index_dir_tail), std::move(index_base));
            break;
        }
        case WalCommandType::DROP_INDEX: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropIndex>(db_name, table_name, index_name);
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
            cmd =
                MakeShared<WalCmdCompact>(std::move(db_name), std::move(table_name), std::move(new_segment_infos), std::move(deprecated_segment_ids));
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
            cmd = MakeShared<WalCmdOptimize>(std::move(db_name), std::move(table_name), std::move(index_name), std::move(params));
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
            cmd = MakeShared<WalCmdDumpIndex>(std::move(db_name),
                                              std::move(table_name),
                                              std::move(index_name),
                                              segment_id,
                                              std::move(chunk_infos),
                                              std::move(old_chunk_ids));
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

bool WalCmdCreateTable::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateTable *>(&other);
    return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && IsEqual(table_dir_tail_, other_cmd->table_dir_tail_) &&
           table_def_.get() != nullptr && other_cmd->table_def_.get() != nullptr && *table_def_ == *other_cmd->table_def_;
}

bool WalCmdCreateIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ &&
           IsEqual(index_dir_tail_, other_cmd->index_dir_tail_) && index_base_.get() != nullptr && other_cmd->index_base_.get() != nullptr &&
           *index_base_ == *other_cmd->index_base_;
}

bool WalCmdDropIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ && index_name_ == other_cmd->index_name_;
}

bool WalCmdImport::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdImport *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name_, other_cmd->db_name_) || !IsEqual(table_name_, other_cmd->table_name_) ||
        segment_info_ != other_cmd->segment_info_) {
        return false;
    }
    return true;
}

bool WalCmdAppend::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdAppend *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name_, other_cmd->db_name_) || !IsEqual(table_name_, other_cmd->table_name_))
        return false;
    return true;
}

bool WalCmdDelete::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDelete *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name_, other_cmd->db_name_) || !IsEqual(table_name_, other_cmd->table_name_) ||
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

bool WalCmdSetSegmentStatusSealed::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdSetSegmentStatusSealed *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name_, other_cmd->db_name_) || !IsEqual(table_name_, other_cmd->table_name_) ||
        segment_id_ != other_cmd->segment_id_ || !IsEqual(segment_filter_binary_data_, other_cmd->segment_filter_binary_data_)) {
        return false;
    }
    if (block_filter_binary_data_ != other_cmd->block_filter_binary_data_) {
        return false;
    }
    return true;
}

bool WalCmdUpdateSegmentBloomFilterData::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdUpdateSegmentBloomFilterData *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name_, other_cmd->db_name_) || !IsEqual(table_name_, other_cmd->table_name_) ||
        segment_id_ != other_cmd->segment_id_ || !IsEqual(segment_filter_binary_data_, other_cmd->segment_filter_binary_data_)) {
        return false;
    }
    if (block_filter_binary_data_ != other_cmd->block_filter_binary_data_) {
        return false;
    }
    return true;
}

bool WalCmdCheckpoint::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCheckpoint *>(&other);
    return other_cmd != nullptr && max_commit_ts_ == other_cmd->max_commit_ts_ && is_full_checkpoint_ == other_cmd->is_full_checkpoint_;
}

bool WalCmdCompact::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCompact *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name_, other_cmd->db_name_) || !IsEqual(table_name_, other_cmd->table_name_) ||
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

bool WalCmdOptimize::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdOptimize *>(&other);
    return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && IsEqual(table_name_, other_cmd->table_name_) &&
           IsEqual(index_name_, other_cmd->index_name_);
}

bool WalCmdDumpIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDumpIndex *>(&other);
    return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && IsEqual(table_name_, other_cmd->table_name_) &&
           IsEqual(index_name_, other_cmd->index_name_) && segment_id_ == other_cmd->segment_id_ && chunk_infos_ == other_cmd->chunk_infos_ &&
           deprecate_ids_ == other_cmd->deprecate_ids_;
}

i32 WalCmdCreateDatabase::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_dir_tail_.size();
}

i32 WalCmdDropDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size(); }

i32 WalCmdCreateTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_dir_tail_.size() +
           this->table_def_->GetSizeInBytes();
}

i32 WalCmdCreateIndex::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
           this->index_dir_tail_.size() + this->index_base_->GetSizeInBytes();
}

i32 WalCmdDropTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size();
}

i32 WalCmdDropIndex::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
           this->index_name_.size();
}

i32 WalCmdImport::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + segment_info_.GetSizeInBytes();
}

i32 WalCmdAppend::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + block_->GetSizeInBytes();
}

i32 WalCmdDelete::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
           row_ids_.size() * sizeof(RowID);
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

i32 WalCmdUpdateSegmentBloomFilterData::GetSizeInBytes() const {
    i32 sz = sizeof(WalCommandType) + ::infinity::GetSizeInBytes(db_name_) + ::infinity::GetSizeInBytes(table_name_) +
             ::infinity::GetSizeInBytes(segment_id_) + ::infinity::GetSizeInBytes(segment_filter_binary_data_);
    sz += +sizeof(i32); // count of vector
    for (const auto &data : block_filter_binary_data_) {
        sz += sizeof(data.first) + ::infinity::GetSizeInBytes(data.second);
    }
    return sz;
}

i32 WalCmdCheckpoint::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i64) + sizeof(i8) + sizeof(i32) + sizeof(i32) + this->catalog_path_.size() + this->catalog_name_.size();
}

i32 WalCmdCompact::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32);
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

i32 WalCmdDumpIndex::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
               this->index_name_.size() + sizeof(SegmentID) + sizeof(i32);
    for (const auto &chunk_info : this->chunk_infos_) {
        size += chunk_info.GetSizeInBytes();
    }
    size += sizeof(i32) + sizeof(ChunkID) * this->deprecate_ids_.size();
    return size;
}

void WalCmdCreateDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_DATABASE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->db_dir_tail_);
}

void WalCmdDropDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_DATABASE);
    WriteBufAdv(buf, this->db_name_);
}

void WalCmdCreateTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_TABLE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_dir_tail_);
    this->table_def_->WriteAdv(buf);
}

void WalCmdCreateIndex::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_INDEX);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_dir_tail_);
    index_base_->WriteAdv(buf);
}

void WalCmdDropTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_TABLE);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
}

void WalCmdDropIndex::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_INDEX);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
}

void WalCmdImport::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::IMPORT);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    this->segment_info_.WriteBufferAdv(buf);
}

void WalCmdAppend::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::APPEND);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
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

void WalCmdCheckpoint::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CHECKPOINT);
    WriteBufAdv(buf, this->max_commit_ts_);
    WriteBufAdv(buf, i8(this->is_full_checkpoint_));
    WriteBufAdv(buf, this->catalog_path_);
    WriteBufAdv(buf, this->catalog_name_);
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

String WalCmdCreateDatabase::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "db dir: " << db_dir_tail_ << std::endl;
    return ss.str();
}

String WalCmdDropDatabase::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    return ss.str();
}

String WalCmdCreateTable::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_def_->ToString() << std::endl;
    ss << "table dir: " << table_dir_tail_ << std::endl;
    return ss.str();
}

String WalCmdDropTable::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    return ss.str();
}

String WalCmdCreateIndex::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "index def: " << index_base_->ToString() << std::endl;
    return ss.str();
}

String WalCmdDropIndex::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "index name: " << index_name_ << std::endl;
    return ss.str();
}

String WalCmdImport::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    auto &segment_info = segment_info_;
    ss << segment_info.ToString() << std::endl;
    return ss.str();
}

String WalCmdAppend::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << block_->ToBriefString();
    return ss.str();
}

String WalCmdDelete::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "delete row cout: " << row_ids_.size() << std::endl;
    return ss.str();
}

String WalCmdSetSegmentStatusSealed::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "segment id: " << segment_id_ << std::endl;
    return ss.str();
}

String WalCmdUpdateSegmentBloomFilterData::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "segment id: " << segment_id_ << std::endl;
    return ss.str();
}

String WalCmdCheckpoint::ToString() const {
    std::stringstream ss;
    ss << "catalog path: " << fmt::format("{}/{}", catalog_path_, catalog_name_) << std::endl;
    ss << "max commit ts: " << max_commit_ts_ << std::endl;
    ss << "is full checkpoint: " << is_full_checkpoint_ << std::endl;
    return ss.str();
}

String WalCmdCompact::ToString() const {
    std::stringstream ss;
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
    return String();
}

String WalCmdOptimize::ToString() const {
    std::stringstream ss;
    ss << "db name: " << db_name_ << std::endl;
    ss << "table name: " << table_name_ << std::endl;
    ss << "index name: " << index_name_ << std::endl;
    ss << "index parameter: ";
    for (auto &param_ptr : params_) {
        ss << param_ptr->ToString() << " | ";
    }
    ss << std::endl;
    return ss.str();
}

String WalCmdDumpIndex::ToString() const {
    std::stringstream ss;
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
    return ss.str();
}

String WalCmdCreateDatabase::CompactInfo() const {
    return fmt::format("{}: database: {}, dir: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, db_dir_tail_);
}

String WalCmdDropDatabase::CompactInfo() const { return fmt::format("{}: database: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_); }

String WalCmdCreateTable::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, dir: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_def_->ToString(),
                       table_dir_tail_);
}

String WalCmdDropTable::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_);
}

String WalCmdCreateIndex::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, index: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       index_base_->ToString());
}

String WalCmdDropIndex::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, index: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_, index_name_);
}

String WalCmdImport::CompactInfo() const {
    auto &segment_info = segment_info_;
    return fmt::format("{}: database: {}, table: {}, segment: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       segment_info.ToString());
}

String WalCmdAppend::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, block: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       db_name_,
                       table_name_,
                       block_->ToBriefString());
}

String WalCmdDelete::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, deleted: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_, row_ids_.size());
}

String WalCmdSetSegmentStatusSealed::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, segment: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_, segment_id_);
}

String WalCmdUpdateSegmentBloomFilterData::CompactInfo() const {
    return fmt::format("{}: database: {}, table: {}, segment: {}", WalCmd::WalCommandTypeToString(GetType()), db_name_, table_name_, segment_id_);
}

String WalCmdCheckpoint::CompactInfo() const {
    return fmt::format("{}: path: {}, max_commit_ts: {}, full_checkpoint: {}",
                       WalCmd::WalCommandTypeToString(GetType()),
                       fmt::format("{}/{}", catalog_path_, catalog_name_),
                       max_commit_ts_,
                       is_full_checkpoint_);
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

    return ss.str();
}

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

SharedPtr<WalEntry> WalEntry::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_end = ptr + max_bytes;
    if (max_bytes <= 0) {
        String error_message = "ptr goes out of range when reading WalEntry";
        LOG_WARN(error_message);
        return nullptr;
    }
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    auto *header = (WalEntryHeader *)ptr;
    entry->size_ = header->size_;
    entry->checksum_ = header->checksum_;
    entry->txn_id_ = header->txn_id_;
    entry->commit_ts_ = header->commit_ts_;
    i32 size2 = *(i32 *)(ptr + entry->size_ - sizeof(i32));
    if (entry->size_ != size2) {
        return nullptr;
    }
    header->checksum_ = 0;
    u32 checksum2 = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(ptr), entry->size_);
    if (entry->checksum_ != checksum2) {
        return nullptr;
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

bool WalEntry::IsCheckPoint(WalCmdCheckpoint *&checkpoint_cmd) const {
    for (auto &cmd : cmds_) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            assert(cmds_.size() == 1);
            checkpoint_cmd = static_cast<WalCmdCheckpoint *>(cmd.get());
            return true;
        }
    }
    return false;
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
    return ss.str();
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
    return ss.str();
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
        case WalCommandType::DROP_DATABASE:
            command = "DROP_DATABASE";
            break;
        case WalCommandType::CREATE_TABLE:
            command = "CREATE_TABLE";
            break;
        case WalCommandType::DROP_TABLE:
            command = "DROP_TABLE";
            break;
        case WalCommandType::ALTER_INFO:
            command = "ALTER_INFO";
            break;
        case WalCommandType::IMPORT:
            command = "IMPORT";
            break;
        case WalCommandType::APPEND:
            command = "APPEND";
            break;
        case WalCommandType::DELETE:
            command = "DELETE";
            break;
        case WalCommandType::SET_SEGMENT_STATUS_SEALED:
            command = "SET_SEGMENT_STATUS_SEALED";
            break;
        case WalCommandType::UPDATE_SEGMENT_BLOOM_FILTER_DATA:
            command = "UPDATE_SEGMENT_BLOOM_FILTER_DATA";
            break;
        case WalCommandType::CHECKPOINT:
            command = "CHECKPOINT";
            break;
        case WalCommandType::CREATE_INDEX:
            command = "CREATE_INDEX";
            break;
        case WalCommandType::DROP_INDEX:
            command = "DROP_INDEX";
            break;
        case WalCommandType::COMPACT:
            command = "COMPACT";
            break;
        case WalCommandType::OPTIMIZE:
            command = "OPTIMIZE";
            break;
        case WalCommandType::DUMP_INDEX:
            command = "DUMP_INDEX";
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
        i32 entry_size = *(i32 *)(buf_.data() + off_ - sizeof(i32));
        if ((SizeT)entry_size > off_) {
            return nullptr;
        }
        char *ptr = buf_.data() + off_ - (SizeT)entry_size;
        auto entry = WalEntry::ReadAdv(ptr, entry_size);
        if (entry.get() != nullptr) {
            off_ -= entry_size;
        }
        return entry;
    } else {
        assert(off_ < buf_.size());
        i32 entry_size = *(i32 *)(buf_.data() + off_);
        if (off_ + (SizeT)entry_size > buf_.size()) {
            return nullptr;
        }
        char *ptr = buf_.data() + off_;
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
                WalCmdCheckpoint *checkpoint_cmd = nullptr;
                if (entry->IsCheckPoint(checkpoint_cmd)) {
                    found_checkpoint = true;
                }
            } else {
                bad_offset = iter_->GetOffset();
                break;
            }
        }
        if (bad_offset != i64(-1)) {
            String error_message = fmt::format("Found bad wal entry {}@{}", *it, bad_offset);
            LOG_WARN(error_message);
            LocalFileSystem fs;
            if (bad_offset == 0) {
                fs.DeleteFile(*it);
                error_message = fmt::format("Removed wal log {}", *it);
                LOG_WARN(error_message);
                ++it;
                for (SizeT i = 0; i <= file_num; ++i) {
                    wal_list_.pop_front();
                }
                file_num = 0;
            } else {
                fs.Truncate(*it, bad_offset);
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

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

namespace infinity {

bool WalSegmentInfo::operator==(const WalSegmentInfo &other) const {
    return segment_dir_ == other.segment_dir_ && segment_id_ == other.segment_id_ && block_entries_size_ == other.block_entries_size_ &&
           block_capacity_ == other.block_capacity_ && last_block_row_count_ == other.last_block_row_count_ &&
           have_rough_filter_ == other.have_rough_filter_ && segment_filter_binary_data_ == other.segment_filter_binary_data_ &&
           block_filter_binary_data_ == other.block_filter_binary_data_;
}

i32 WalSegmentInfo::GetSizeInBytes() const {
    i32 sz = sizeof(i32) + segment_dir_.size() + sizeof(SegmentID) + sizeof(u16) + sizeof(u32) + sizeof(u16);
    sz += sizeof(have_rough_filter_);
    if (have_rough_filter_) {
        sz += ::infinity::GetSizeInBytes(segment_filter_binary_data_);
        i32 block_filter_string_data_size = block_filter_binary_data_.size();
        sz += sizeof(block_filter_string_data_size);
        for (i32 i = 0; i < block_filter_string_data_size; ++i) {
            const Pair<BlockID, String> &data = block_filter_binary_data_[i];
            sz += ::infinity::GetSizeInBytes(data.first) + ::infinity::GetSizeInBytes(data.second);
        }
    }
    return sz;
}

void WalSegmentInfo::WriteBufferAdv(char *&buf) const {
    WriteBufAdv(buf, segment_dir_);
    WriteBufAdv(buf, segment_id_);
    WriteBufAdv(buf, block_entries_size_);
    WriteBufAdv(buf, block_capacity_);
    WriteBufAdv(buf, last_block_row_count_);
    WriteBufAdv(buf, have_rough_filter_);
    if (have_rough_filter_) {
        WriteBufAdv(buf, segment_filter_binary_data_);
        i32 block_filter_string_data_size = block_filter_binary_data_.size();
        WriteBufAdv(buf, block_filter_string_data_size);
        for (i32 i = 0; i < block_filter_string_data_size; ++i) {
            const Pair<BlockID, String> &data = block_filter_binary_data_[i];
            WriteBufAdv(buf, data.first);
            WriteBufAdv(buf, data.second);
        }
    }
}

WalSegmentInfo WalSegmentInfo::ReadBufferAdv(char *&ptr) {
    String segment_dir = ReadBufAdv<String>(ptr);
    SegmentID segment_id = ReadBufAdv<SegmentID>(ptr);
    u16 block_entries_size = ReadBufAdv<u16>(ptr);
    u32 block_capacity = ReadBufAdv<u32>(ptr);
    u16 last_block_row_count = ReadBufAdv<u16>(ptr);
    u8 have_rough_filter = ReadBufAdv<u8>(ptr);
    String segment_filter_binary_data;
    Vector<Pair<BlockID, String>> block_filter_binary_data;
    if (have_rough_filter) {
        segment_filter_binary_data = ReadBufAdv<String>(ptr);
        i32 count = ReadBufAdv<i32>(ptr);
        block_filter_binary_data.resize(count);
        for (auto &data : block_filter_binary_data) {
            data.first = ReadBufAdv<BlockID>(ptr);
            data.second = ReadBufAdv<String>(ptr);
        }
    }
    return {std::move(segment_dir),
            segment_id,
            block_entries_size,
            block_capacity,
            last_block_row_count,
            have_rough_filter,
            std::move(segment_filter_binary_data),
            std::move(block_filter_binary_data)};
}

SharedPtr<WalCmd> WalCmd::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_end = ptr + max_bytes;
    SharedPtr<WalCmd> cmd = nullptr;
    auto cmd_type = ReadBufAdv<WalCommandType>(ptr);
    switch (cmd_type) {
        case WalCommandType::CREATE_DATABASE: {
            String db_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdCreateDatabase>(db_name);
            break;
        }
        case WalCommandType::DROP_DATABASE: {
            String db_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropDatabase>(db_name);
            break;
        }
        case WalCommandType::CREATE_TABLE: {
            String db_name = ReadBufAdv<String>(ptr);
            SharedPtr<TableDef> table_def = TableDef::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdCreateTable>(db_name, table_def);
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
            SharedPtr<DataBlock> block = block->ReadAdv(ptr, ptr_end - ptr);
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
        case WalCommandType::SET_SEGMENT_SEALED: {
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
            cmd = MakeShared<WalCmdSetSegmentSealed>(std::move(db_name),
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
            cmd = MakeShared<WalCmdCheckpoint>(max_commit_ts, is_full_checkpoint, catalog_path);
            break;
        }
        case WalCommandType::CREATE_INDEX: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String table_index_dir = ReadBufAdv<String>(ptr);
            SharedPtr<IndexBase> index_base = IndexBase::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdCreateIndex>(db_name, table_name, table_index_dir, index_base);
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
        default:
            UnrecoverableError(fmt::format("UNIMPLEMENTED ReadAdv for WalCmd command {}", int(cmd_type)));
    }
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        UnrecoverableError("ptr goes out of range when reading WalCmd");
    }
    return cmd;
}

bool WalCmdCreateTable::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateTable *>(&other);
    return other_cmd != nullptr && IsEqual(db_name_, other_cmd->db_name_) && table_def_.get() != nullptr && other_cmd->table_def_.get() != nullptr &&
           *table_def_ == *other_cmd->table_def_;
}

bool WalCmdCreateIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ && index_base_.get() != nullptr &&
           other_cmd->index_base_.get() != nullptr && *index_base_ == *other_cmd->index_base_ && table_index_dir_ == other_cmd->table_index_dir_;
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

bool WalCmdSetSegmentSealed::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdSetSegmentSealed *>(&other);
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

i32 WalCmdCreateDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size(); }

i32 WalCmdDropDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size(); }

i32 WalCmdCreateTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + this->table_def_->GetSizeInBytes();
}

i32 WalCmdCreateIndex::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
           this->table_index_dir_.size() + this->index_base_->GetSizeInBytes();
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

i32 WalCmdSetSegmentSealed::GetSizeInBytes() const {
    i32 sz = sizeof(WalCommandType) + ::infinity::GetSizeInBytes(db_name_) + ::infinity::GetSizeInBytes(table_name_) +
             ::infinity::GetSizeInBytes(segment_id_) + ::infinity::GetSizeInBytes(segment_filter_binary_data_);
    sz += +sizeof(i32); // count of vector
    for (const auto &data : block_filter_binary_data_) {
        sz += sizeof(data.first) + ::infinity::GetSizeInBytes(data.second);
    }
    return sz;
}

i32 WalCmdCheckpoint::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i64) + sizeof(i8) + sizeof(i32) + this->catalog_path_.size(); }

i32 WalCmdCompact::GetSizeInBytes() const {
    i32 size = sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32);
    for (const auto &segment_info : this->new_segment_infos_) {
        size += segment_info.GetSizeInBytes();
    }
    return size + sizeof(i32) + this->deprecated_segment_ids_.size() * sizeof(SegmentID);
}

void WalCmdCreateDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_DATABASE);
    WriteBufAdv(buf, this->db_name_);
}

void WalCmdDropDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_DATABASE);
    WriteBufAdv(buf, this->db_name_);
}

void WalCmdCreateTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_TABLE);
    WriteBufAdv(buf, this->db_name_);
    this->table_def_->WriteAdv(buf);
}

void WalCmdCreateIndex::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_INDEX);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->table_index_dir_);
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

void WalCmdSetSegmentSealed::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::SET_SEGMENT_SEALED);
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
        UnrecoverableError("ptr goes out of range when reading WalEntry");
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
            UnrecoverableError("ptr goes out of range when reading WalEntry");
        }
        SharedPtr<WalCmd> cmd = WalCmd::ReadAdv(ptr, max_bytes);
        entry->cmds_.push_back(cmd);
    }
    ptr += sizeof(i32);
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        UnrecoverableError("ptr goes out of range when reading WalEntry");
    }
    return entry;
}

Pair<i64, String> WalEntry::GetCheckpointInfo() const {
    for (const auto &cmd : cmds_) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            auto checkpoint_cmd = dynamic_cast<const WalCmdCheckpoint *>(cmd.get());
            return {checkpoint_cmd->max_commit_ts_, checkpoint_cmd->catalog_path_};
        }
    }
    return {-1, ""};
}

bool WalEntry::IsCheckPoint() const {
    for (const auto &cmd : cmds_) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            return true;
        }
    }
    return false;
}

bool WalEntry::IsFullCheckPoint() const {
    for (const auto &cmd : cmds_) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT && dynamic_cast<const WalCmdCheckpoint *>(cmd.get())->is_full_checkpoint_) {
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
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            auto checkpoint_cmd = dynamic_cast<const WalCmdCheckpoint *>(cmd.get());
            ss << "catalog path: " << checkpoint_cmd->catalog_path_ << std::endl;
            ss << "max commit ts: " << checkpoint_cmd->max_commit_ts_ << std::endl;
            ss << "is full checkpoint: " << checkpoint_cmd->is_full_checkpoint_ << std::endl;
        } else if (cmd->GetType() == WalCommandType::IMPORT) {
            auto import_cmd = dynamic_cast<const WalCmdImport *>(cmd.get());
            ss << "db name: " << import_cmd->db_name_ << std::endl;
            ss << "table name: " << import_cmd->table_name_ << std::endl;
            auto &segment_info = import_cmd->segment_info_;
            ss << "segment dir: " << segment_info.segment_dir_ << std::endl;
            ss << "segment id: " << segment_info.segment_id_ << std::endl;
            ss << "block entries size: " << segment_info.block_entries_size_ << std::endl;
            ss << "block capacity: " << segment_info.block_capacity_ << std::endl;
            ss << "last block row count" << segment_info.last_block_row_count_ << std::endl;
        } else if (cmd->GetType() == WalCommandType::APPEND) {
            auto append_cmd = dynamic_cast<const WalCmdAppend *>(cmd.get());
            ss << "db name: " << append_cmd->db_name_ << std::endl;
            ss << "table name: " << append_cmd->table_name_ << std::endl;
            ss << append_cmd->block_->ToString();
        } else if (cmd->GetType() == WalCommandType::DELETE) {
            auto delete_cmd = dynamic_cast<const WalCmdDelete *>(cmd.get());
            ss << "db name: " << delete_cmd->db_name_ << std::endl;
            ss << "table name: " << delete_cmd->table_name_ << std::endl;
            ss << "row ids: ";
            for (const auto &row_id : delete_cmd->row_ids_) {
                ss << row_id.ToString() << " ";
            }
            ss << std::endl;
        } else if (cmd->GetType() == WalCommandType::SET_SEGMENT_SEALED) {
            auto set_sealed_cmd = dynamic_cast<const WalCmdSetSegmentSealed *>(cmd.get());
            ss << "db name: " << set_sealed_cmd->db_name_ << std::endl;
            ss << "table name: " << set_sealed_cmd->table_name_ << std::endl;
            ss << "segment id: " << set_sealed_cmd->segment_id_ << std::endl;
        } else if (cmd->GetType() == WalCommandType::CREATE_INDEX) {
            auto create_index_cmd = dynamic_cast<const WalCmdCreateIndex *>(cmd.get());
            ss << "db name: " << create_index_cmd->db_name_ << std::endl;
            ss << "table name: " << create_index_cmd->table_name_ << std::endl;
            ss << "table index dir: " << create_index_cmd->table_index_dir_ << std::endl;
            ss << "index def: " << create_index_cmd->index_base_->ToString() << std::endl;
        }
    }
    ss << "========================" << std::endl;
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
        case WalCommandType::SET_SEGMENT_SEALED:
            command = "SET_SEGMENT_SEALED";
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
        default:
            UnrecoverableError("Unknown command type");
    }
    return command;
}

WalEntryIterator WalEntryIterator::Make(const String &wal_path) {
    std::ifstream ifs(wal_path.c_str(), std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        UnrecoverableError("Wal open failed");
    }
    auto wal_size = ifs.tellg();
    Vector<char> buf(wal_size);
    ifs.seekg(0, std::ios::beg);
    ifs.read(buf.data(), wal_size);
    ifs.close();

    return WalEntryIterator(std::move(buf), wal_size);
}

SharedPtr<WalEntry> WalEntryIterator::Next() {
    if (end_ > buf_.data()) {
        i32 entry_size;
        std::memcpy(&entry_size, end_ - sizeof(i32), sizeof(entry_size));
        end_ = end_ - entry_size;
        auto entry = WalEntry::ReadAdv(end_, entry_size);
        end_ = end_ - entry_size;
        return entry;
    } else {
        return nullptr;
    }
}

SharedPtr<WalEntry> WalListIterator::Next() {
    if (iter_.get() != nullptr) {
        SharedPtr<WalEntry> entry = iter_->Next();

        if (entry.get() != nullptr) {
            return entry;
        }
    }
    if (!wal_deque_.empty()) {
        iter_ = MakeUnique<WalEntryIterator>(WalEntryIterator::Make(wal_deque_.front()));
        wal_deque_.pop_front();

        return Next();
    } else {
        return nullptr;
    }
}

} // namespace infinity

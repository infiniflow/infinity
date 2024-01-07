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

module wal;

import stl;
import serialize;
import table_def;
import data_block;

import infinity_exception;
import parser;
import third_party;
import index_def;
import crc;

namespace infinity {

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
            String segment_dir = ReadBufAdv<String>(ptr);
            u32 segment_id = ReadBufAdv<u32>(ptr);
            u16 block_entries_size = ReadBufAdv<u16>(ptr);
            Vector<u16> row_counts;
            for (u16 i = 0; i < block_entries_size; ++i) {
                auto row_count = ReadBufAdv<u16>(ptr);
                row_counts.push_back(row_count);
            }
            cmd = MakeShared<WalCmdImport>(db_name, table_name, segment_dir, segment_id, block_entries_size, row_counts);
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
            SharedPtr<IndexDef> index_def = IndexDef::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdCreateIndex>(db_name, table_name, index_def);
            break;
        }
        case WalCommandType::DROP_INDEX: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            String index_name = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdDropIndex>(db_name, table_name, index_name);
            break;
        }
        default:
            Error<StorageException>(fmt::format("UNIMPLEMENTED ReadAdv for WalCmd command {}", int(cmd_type)));
    }
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        Error<StorageException>("ptr goes out of range when reading WalCmd");
    }
    return cmd;
}

bool WalCmdCreateTable::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateTable *>(&other);
    return other_cmd != nullptr && IsEqual(db_name, other_cmd->db_name) && table_def.get() != nullptr && other_cmd->table_def.get() != nullptr &&
           *table_def == *other_cmd->table_def;
}

bool WalCmdCreateIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ && index_def_.get() != nullptr &&
           other_cmd->index_def_.get() != nullptr && *index_def_ == *other_cmd->index_def_;
}

bool WalCmdDropIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDropIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ && index_name_ == other_cmd->index_name_;
}

bool WalCmdImport::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdImport *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name, other_cmd->db_name) || !IsEqual(table_name, other_cmd->table_name) ||
        segment_dir != other_cmd->segment_dir || segment_id != other_cmd->segment_id || block_entries_size != other_cmd->block_entries_size ||
        row_counts_.size() != other_cmd->row_counts_.size())
        return false;
    return true;
}

bool WalCmdAppend::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdAppend *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name, other_cmd->db_name) || !IsEqual(table_name, other_cmd->table_name))
        return false;
    return true;
}

bool WalCmdDelete::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDelete *>(&other);
    if (other_cmd == nullptr || !IsEqual(db_name, other_cmd->db_name) || !IsEqual(table_name, other_cmd->table_name) ||
        row_ids.size() != other_cmd->row_ids.size()) {
        return false;
    }
    for (SizeT i = 0; i < row_ids.size(); i++) {
        if (row_ids[i] != other_cmd->row_ids[i]) {
            return false;
        }
    }
    return true;
}

bool WalCmdCheckpoint::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCheckpoint *>(&other);
    return other_cmd != nullptr && max_commit_ts_ == other_cmd->max_commit_ts_ && is_full_checkpoint_ == other_cmd->is_full_checkpoint_;
}

i32 WalCmdCreateDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size(); }

i32 WalCmdDropDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size(); }

i32 WalCmdCreateTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + this->table_def->GetSizeInBytes();
}

i32 WalCmdCreateIndex::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + this->index_def_->GetSizeInBytes();
}

i32 WalCmdDropTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(i32) + this->table_name.size();
}

i32 WalCmdDropIndex::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() + sizeof(i32) +
           this->index_name_.size();
}

i32 WalCmdImport::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(i32) + this->table_name.size() + sizeof(i32) +
           this->segment_dir.size() + sizeof(u32) + sizeof(u16) + this->row_counts_.size() * sizeof(u16);
}

i32 WalCmdAppend::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(i32) + this->table_name.size() + block->GetSizeInBytes();
}

i32 WalCmdDelete::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(i32) + this->table_name.size() + sizeof(i32) +
           row_ids.size() * sizeof(RowID);
}

i32 WalCmdCheckpoint::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i64) + sizeof(i8) + sizeof(i32) + this->catalog_path_.size(); }

void WalCmdCreateDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_DATABASE);
    WriteBufAdv(buf, this->db_name);
}

void WalCmdDropDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_DATABASE);
    WriteBufAdv(buf, this->db_name);
}

void WalCmdCreateTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_TABLE);
    WriteBufAdv(buf, this->db_name);
    this->table_def->WriteAdv(buf);
}

void WalCmdCreateIndex::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_INDEX);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    index_def_->WriteAdv(buf);
}

void WalCmdDropTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_TABLE);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
}

void WalCmdDropIndex::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_INDEX);
    WriteBufAdv(buf, this->db_name_);
    WriteBufAdv(buf, this->table_name_);
    WriteBufAdv(buf, this->index_name_);
}

void WalCmdImport::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::IMPORT);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
    WriteBufAdv(buf, this->segment_dir);
    WriteBufAdv(buf, this->segment_id);
    WriteBufAdv(buf, this->block_entries_size);
    for (const auto &row_count : this->row_counts_) {
        WriteBufAdv(buf, row_count);
    }
}

void WalCmdAppend::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::APPEND);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
    block->WriteAdv(buf);
}

void WalCmdDelete::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DELETE);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
    WriteBufAdv(buf, static_cast<i32>(this->row_ids.size()));
    SizeT row_count = this->row_ids.size();
    for (SizeT idx = 0; idx < row_count; ++idx) {
        const auto &row_id = this->row_ids[idx];
        WriteBufAdv(buf, row_id);
    }
}

void WalCmdCheckpoint::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CHECKPOINT);
    WriteBufAdv(buf, this->max_commit_ts_);
    WriteBufAdv(buf, i8(this->is_full_checkpoint_));
    WriteBufAdv(buf, this->catalog_path_);
}

bool WalEntry::operator==(const WalEntry &other) const {
    if (this->txn_id != other.txn_id || this->commit_ts != other.commit_ts || this->cmds.size() != other.cmds.size()) {
        return false;
    }
    for (u32 i = 0; i < this->cmds.size(); i++) {
        const SharedPtr<WalCmd> &cmd1 = this->cmds[i];
        const SharedPtr<WalCmd> &cmd2 = other.cmds[i];
        if (cmd1.get() == nullptr || cmd2.get() == nullptr || (*cmd1).operator!=(*cmd2)) {
            return false;
        }
    }
    return true;
}

bool WalEntry::operator!=(const WalEntry &other) const { return !operator==(other); }

i32 WalEntry::GetSizeInBytes() const {
    i32 size = sizeof(WalEntryHeader) + sizeof(i32);
    SizeT cmd_count = cmds.size();
    for (SizeT idx = 0; idx < cmd_count; ++idx) {
        const auto &cmd = cmds[idx];
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
    Memcpy(ptr, this, sizeof(WalEntryHeader));
    ptr += sizeof(WalEntryHeader);
    WriteBufAdv(ptr, static_cast<i32>(cmds.size()));
    SizeT cmd_count = cmds.size();
    for (SizeT idx = 0; idx < cmd_count; ++idx) {
        const auto &cmd = cmds[idx];
        cmd->WriteAdv(ptr);
    }
    i32 size = ptr - saved_ptr + sizeof(i32);
    WriteBufAdv(ptr, size);
    auto *header = (WalEntryHeader *)saved_ptr;
    header->size = size;
    header->checksum = 0;
    // CRC32IEEE is equivalent to boost::crc_32_type on
    // little-endian machine.
    header->checksum = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(saved_ptr), size);
}

SharedPtr<WalEntry> WalEntry::ReadAdv(char *&ptr, i32 max_bytes) {
    char *const ptr_end = ptr + max_bytes;
    if (max_bytes <= 0) {
        Error<StorageException>("ptr goes out of range when reading WalEntry");
    }
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    auto *header = (WalEntryHeader *)ptr;
    entry->size = header->size;
    entry->checksum = header->checksum;
    entry->txn_id = header->txn_id;
    entry->commit_ts = header->commit_ts;
    i32 size2 = *(i32 *)(ptr + entry->size - sizeof(i32));
    if (entry->size != size2) {
        return nullptr;
    }
    header->checksum = 0;
    u32 checksum2 = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(ptr), entry->size);
    if (entry->checksum != checksum2) {
        return nullptr;
    }
    ptr += sizeof(WalEntryHeader);
    i32 cnt = ReadBufAdv<i32>(ptr);
    for (i32 i = 0; i < cnt; i++) {
        max_bytes = ptr_end - ptr;
        if (max_bytes <= 0) {
            Error<StorageException>("ptr goes out of range when reading WalEntry");
        }
        SharedPtr<WalCmd> cmd = WalCmd::ReadAdv(ptr, max_bytes);
        entry->cmds.push_back(cmd);
    }
    ptr += sizeof(i32);
    max_bytes = ptr_end - ptr;
    if (max_bytes < 0) {
        Error<StorageException>("ptr goes out of range when reading WalEntry");
    }
    return entry;
}

Pair<i64, String> WalEntry::GetCheckpointInfo() const {
    for (const auto &cmd : cmds) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            auto checkpoint_cmd = dynamic_cast<const WalCmdCheckpoint *>(cmd.get());
            return {checkpoint_cmd->max_commit_ts_, checkpoint_cmd->catalog_path_};
        }
    }
    return {-1, ""};
}
bool WalEntry::IsCheckPoint() const {
    for (const auto &cmd : cmds) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            return true;
        }
    }
    return false;
}

bool WalEntry::IsFullCheckPoint() const {
    for (const auto &cmd : cmds) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT && dynamic_cast<const WalCmdCheckpoint *>(cmd.get())->is_full_checkpoint_) {
            return true;
        }
    }
    return false;
}
String WalEntry::ToString() const {
    std::stringstream ss;
    ss << "wal entry" << std::endl;
    ss << "wal entry header" << std::endl;
    ss << "txn_id: " << txn_id << std::endl;
    ss << "commit_ts: " << commit_ts << std::endl;
    ss << "size: " << size << std::endl;
    for (const auto &cmd : cmds) {
        ss << "[" << WalCmd::WalCommandTypeToString(cmd->GetType()) << "]" << std::endl;
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            auto checkpoint_cmd = dynamic_cast<const WalCmdCheckpoint *>(cmd.get());
            ss << "catalog path: " << checkpoint_cmd->catalog_path_ << std::endl;
            ss << "max commit ts: " << checkpoint_cmd->max_commit_ts_ << std::endl;
            ss << "is full checkpoint: " << checkpoint_cmd->is_full_checkpoint_ << std::endl;
        }
        if (cmd->GetType() == WalCommandType::IMPORT) {
            auto import_cmd = dynamic_cast<const WalCmdImport *>(cmd.get());
            ss << "db name: " << import_cmd->db_name << std::endl;
            ss << "table name: " << import_cmd->table_name << std::endl;
            ss << "segment dir: " << import_cmd->segment_dir << std::endl;
            ss << "segment id: " << import_cmd->segment_id << std::endl;
            ss << "block entries size: " << import_cmd->block_entries_size << std::endl;
        }
        if (cmd->GetType() == WalCommandType::APPEND) {
            auto append_cmd = dynamic_cast<const WalCmdAppend *>(cmd.get());
            ss << "db name: " << append_cmd->db_name << std::endl;
            ss << "table name: " << append_cmd->table_name << std::endl;
            ss << append_cmd->block->ToString();
        }
    }
    return ss.str();
}

String WalCmd::WalCommandTypeToString(WalCommandType type) {
    switch (type) {
        case WalCommandType::INVALID:
            return "INVALID";
        case WalCommandType::CREATE_DATABASE:
            return "CREATE_DATABASE";
        case WalCommandType::DROP_DATABASE:
            return "DROP_DATABASE";
        case WalCommandType::CREATE_TABLE:
            return "CREATE_TABLE";
        case WalCommandType::DROP_TABLE:
            return "DROP_TABLE";
        case WalCommandType::ALTER_INFO:
            return "ALTER_INFO";
        case WalCommandType::IMPORT:
            return "IMPORT";
        case WalCommandType::APPEND:
            return "APPEND";
        case WalCommandType::DELETE:
            return "DELETE";
        case WalCommandType::CHECKPOINT:
            return "CHECKPOINT";
        case WalCommandType::CREATE_INDEX:
            return "CREATE_INDEX";
        case WalCommandType::DROP_INDEX:
            return "DROP_INDEX";
        default:
            Error<StorageException>("Unknown command type");
    }
}

WalEntryIterator WalEntryIterator::Make(const String &wal_path) {
    std::ifstream ifs(wal_path.c_str(), std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        Error<StorageException>("Wal open failed");
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
        Memcpy(&entry_size, end_ - sizeof(i32), sizeof(entry_size));
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

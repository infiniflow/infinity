#include "wal_entry.h"

#include "common/utility/crc.hpp"
#include "common/utility/serializable.h"
#include "storage/data_block.h"
#include "storage/table_def.h"

#include <cstdint>
#include <cstring>

namespace infinity {

SharedPtr<WalCmd> WalCmd::ReadAdv(char *&ptr, int32_t maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    SharedPtr<WalCmd> cmd = nullptr;
    WalCommandType cmd_type = ReadBufAdv<WalCommandType>(ptr);
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
            cmd = MakeShared<WalCmdImport>(db_name, table_name, segment_dir);
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
            int32_t cnt = ReadBufAdv<int32_t>(ptr);
            Vector<RowID> row_ids;
            for (int32_t i = 0; i < cnt; ++i) {
                RowID row_id = ReadBufAdv<RowID>(ptr);
                row_ids.push_back(row_id);
            }
            cmd = MakeShared<WalCmdDelete>(db_name, table_name, row_ids);
            break;
        }
        case WalCommandType::CHECKPOINT: {
            int64_t max_commit_ts = ReadBufAdv<int64_t>(ptr);
            String catalog_path = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdCheckpoint>(max_commit_ts, catalog_path);
            break;
        }
        default:
            StorageError(fmt::format("UNIMPLEMENTED ReadAdv for WalCmd command {}", int(cmd_type)));
    }
    maxbytes = ptr_end - ptr;
    StorageAssert(maxbytes >= 0, "ptr goes out of range when reading WalCmd");
    return cmd;
}

bool WalCmdCreateTable::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateTable *>(&other);
    return other_cmd != nullptr && db_name == other_cmd->db_name && table_def != nullptr && other_cmd->table_def != nullptr &&
           *table_def == *other_cmd->table_def;
}

bool WalCmdCreateIndex::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateIndex *>(&other);
    return other_cmd != nullptr && db_name_ == other_cmd->db_name_ && table_name_ == other_cmd->table_name_ && index_def_ != nullptr &&
           other_cmd->index_def_ != nullptr && *index_def_ == *other_cmd->index_def_;
}

bool WalCmdImport::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdImport *>(&other);
    if (other_cmd == nullptr || db_name != other_cmd->db_name || table_name != other_cmd->table_name || segment_dir != other_cmd->segment_dir)
        return false;
    return true;
}

bool WalCmdAppend::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdAppend *>(&other);
    if (other_cmd == nullptr || db_name != other_cmd->db_name || table_name != other_cmd->table_name)
        return false;
    return true;
}

bool WalCmdDelete::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDelete *>(&other);
    if (other_cmd == nullptr || db_name != other_cmd->db_name || table_name != other_cmd->table_name || row_ids.size() != other_cmd->row_ids.size()) {
        return false;
    }
    for (size_t i = 0; i < row_ids.size(); i++) {
        if (row_ids[i] != other_cmd->row_ids[i]) {
            return false;
        }
    }
    return true;
}

bool WalCmdCheckpoint::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCheckpoint *>(&other);
    return other_cmd != nullptr && max_commit_ts_ == other_cmd->max_commit_ts_;
}

int32_t WalCmdCreateDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size(); }

int32_t WalCmdDropDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size(); }

int32_t WalCmdCreateTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size() + this->table_def->GetSizeInBytes();
}

int32_t WalCmdCreateIndex::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name_.size() + sizeof(int32_t) + this->table_name_.size() + sizeof(int32_t) +
           this->index_def_->GetSizeInBytes();
}

int32_t WalCmdDropTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size() + sizeof(int32_t) + this->table_name.size();
}

int32_t WalCmdImport::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size() + sizeof(int32_t) + this->table_name.size() + sizeof(int32_t) +
           this->segment_dir.size();
}

int32_t WalCmdAppend::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size() + sizeof(int32_t) + this->table_name.size() + block->GetSizeInBytes();
}

int32_t WalCmdDelete::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size() + sizeof(int32_t) + this->table_name.size() + sizeof(int32_t) +
           row_ids.size() * sizeof(RowID);
}

int32_t WalCmdCheckpoint::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(int64_t) + sizeof(int32_t) + this->catalog_path_.size(); }

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

void WalCmdImport::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::IMPORT);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
    WriteBufAdv(buf, this->segment_dir);
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
    WriteBufAdv(buf, static_cast<int32_t>(this->row_ids.size()));
    for (const auto &row_id : this->row_ids) {
        WriteBufAdv(buf, row_id);
    }
}

void WalCmdCheckpoint::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CHECKPOINT);
    WriteBufAdv(buf, this->max_commit_ts_);
    WriteBufAdv(buf, this->catalog_path_);
}

bool WalEntry::operator==(const WalEntry &other) const {
    if (this->txn_id != other.txn_id || this->commit_ts != other.commit_ts || this->cmds.size() != other.cmds.size()) {
        return false;
    }
    for (int32_t i = 0; i < this->cmds.size(); i++) {
        const SharedPtr<WalCmd> &cmd1 = this->cmds[i];
        const SharedPtr<WalCmd> &cmd2 = other.cmds[i];
        if (cmd1 == nullptr || cmd2 == nullptr || (*cmd1).operator!=(*cmd2)) {
            return false;
        }
    }
    return true;
}

bool WalEntry::operator!=(const WalEntry &other) const { return !operator==(other); }

int32_t WalEntry::GetSizeInBytes() const {
    int32_t size = sizeof(WalEntryHeader) + sizeof(int32_t);
    for (const auto &cmd : cmds) {
        size += cmd->GetSizeInBytes();
    }
    size += sizeof(int32_t); // pad
    return size;
}

void WalEntry::WriteAdv(char *&ptr) const {
    // An entry is serialized as follows:
    // - WalEntryHeader
    // - number of WalCmd
    // - (repeated) WalCmd
    // - 4 bytes pad
    char *const saved_ptr = ptr;
    memcpy(ptr, this, sizeof(WalEntryHeader));
    ptr += sizeof(WalEntryHeader);
    WriteBufAdv(ptr, static_cast<int32_t>(cmds.size()));
    for (const auto &cmd : cmds) {
        cmd->WriteAdv(ptr);
    }
    int32_t size = ptr - saved_ptr + sizeof(int32_t);
    WriteBufAdv(ptr, size);
    WalEntryHeader *header = (WalEntryHeader *)saved_ptr;
    header->size = size;
    header->checksum = 0;
    // CRC32IEEE is equivalent to boost::crc_32_type on
    // little-endian machine.
    header->checksum = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(saved_ptr), size);
    return;
}

SharedPtr<WalEntry> WalEntry::ReadAdv(char *&ptr, int32_t maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    StorageAssert(maxbytes > 0, "ptr goes out of range when reading WalEntry");
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    WalEntryHeader *header = (WalEntryHeader *)ptr;
    entry->size = header->size;
    entry->checksum = header->checksum;
    entry->txn_id = header->txn_id;
    entry->commit_ts = header->commit_ts;
    int32_t size2 = *(int32_t *)(ptr + entry->size - sizeof(int32_t));
    if (entry->size != size2) {
        return nullptr;
    }
    header->checksum = 0;
    uint32_t checksum2 = CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(ptr), entry->size);
    if (entry->checksum != checksum2) {
        return nullptr;
    }
    ptr += sizeof(WalEntryHeader);
    int32_t cnt = ReadBufAdv<int32_t>(ptr);
    for (size_t i = 0; i < cnt; i++) {
        maxbytes = ptr_end - ptr;
        StorageAssert(maxbytes > 0, "ptr goes out of range when reading WalEntry");
        SharedPtr<WalCmd> cmd = WalCmd::ReadAdv(ptr, maxbytes);
        entry->cmds.push_back(cmd);
    }
    ptr += sizeof(int32_t);
    maxbytes = ptr_end - ptr;
    StorageAssert(maxbytes >= 0, "ptr goes out of range when reading WalEntry");
    return entry;
}

} // namespace infinity

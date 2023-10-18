#include "wal_entry.h"
#include "common/utility/crc.hpp"
#include "common/utility/serializable.h"

#include "storage/data_block.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

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
            ConflictType conflict_type = ReadBufAdv<ConflictType>(ptr);
            cmd = MakeShared<WalCmdDropDatabase>(db_name, conflict_type);
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
            ConflictType conflict_type = ReadBufAdv<ConflictType>(ptr);
            cmd = MakeShared<WalCmdDropTable>(db_name, table_name, conflict_type);
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
            auto max_commit_ts = ReadBufAdv<int64_t>(ptr);
            String catalog_path = ReadBufAdv<String>(ptr);
            cmd = MakeShared<WalCmdCheckpoint>(max_commit_ts, catalog_path);
            break;
        }
        case WalCommandType::CREATE_INDEX: {
            String db_name = ReadBufAdv<String>(ptr);
            String table_name = ReadBufAdv<String>(ptr);
            ConflictType conflict_type = ReadBufAdv<ConflictType>(ptr);
            SharedPtr<IndexDef> index_def = IndexDef::ReadAdv(ptr, ptr_end - ptr);
            cmd = MakeShared<WalCmdCreateIndex>(db_name, table_name, index_def, conflict_type);
            break;
        }
        default:
            StorageError(fmt::format("UNIMPLEMENTED ReadAdv for WalCmd command {}", int(cmd_type)));
    }
    max_bytes = ptr_end - ptr;
    StorageAssert(max_bytes >= 0, "ptr goes out of range when reading WalCmd");
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

i32 WalCmdCreateDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size(); }

i32 WalCmdDropDatabase::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(ConflictType); }

i32 WalCmdCreateTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + this->table_def->GetSizeInBytes();
}

i32 WalCmdCreateIndex::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
           this->index_def_->GetSizeInBytes() + sizeof(ConflictType);
}

i32 WalCmdDropTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(i32) + this->table_name.size() + sizeof(ConflictType);
}

i32 WalCmdImport::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(i32) + this->table_name.size() + sizeof(i32) +
           this->segment_dir.size();
}

i32 WalCmdAppend::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(i32) + this->table_name.size() + block->GetSizeInBytes();
}

i32 WalCmdDelete::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(i32) + this->db_name.size() + sizeof(i32) + this->table_name.size() + sizeof(i32) +
           row_ids.size() * sizeof(RowID);
}

i32 WalCmdCheckpoint::GetSizeInBytes() const { return sizeof(WalCommandType) + sizeof(int64_t) + sizeof(i32) + this->catalog_path_.size(); }

void WalCmdCreateDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CREATE_DATABASE);
    WriteBufAdv(buf, this->db_name);
}

void WalCmdDropDatabase::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_DATABASE);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->conflict_type_);
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
    WriteBufAdv(buf, this->conflict_type_);
    index_def_->WriteAdv(buf);
}

void WalCmdDropTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_TABLE);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
    WriteBufAdv(buf, this->conflict_type_);
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
    WriteBufAdv(buf, static_cast<i32>(this->row_ids.size()));
    for (const auto &row_id : this->row_ids) {
        WriteBufAdv(buf, row_id);
    }
}

void WalCmdCheckpoint::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CHECKPOINT);
    WriteBufAdv(buf, this->max_commit_ts_);
    WriteBufAdv(buf, this->catalog_path_);
}

void WalCmdCheckpoint::Replay(Storage *storage, u64 txn_id, u64 commit_ts) {
    // nothing to do
}
void WalCmdCreateDatabase::Replay(Storage *storage, u64 txn_id, u64 commit_ts) {
    auto result = NewCatalog::CreateDatabase(storage->catalog(), db_name, txn_id, commit_ts, storage->txn_manager());
    if (!result.Success()) {
        StorageError("Wal Replay: Create database failed");
    }
    result.entry_->Commit(commit_ts);
}
void WalCmdCreateTable::Replay(Storage *storage, u64 txn_id, u64 commit_ts) {
    auto db_entry_result = NewCatalog::GetDatabase(storage->catalog(), db_name, txn_id, commit_ts);
    if (!db_entry_result.Success()) {
        StorageError("Wal Replay: Get database failed");
    }
    auto db_entry = dynamic_cast<DBEntry *>(db_entry_result.entry_);
    auto result = DBEntry::CreateTableCollection(db_entry,
                                                 TableCollectionType::kTableEntry,
                                                 table_def->table_name(),
                                                 table_def->columns(),
                                                 txn_id,
                                                 commit_ts,
                                                 nullptr);
    if (!result.Success()) {
        StorageError("Wal Replay: Create table failed");
    }
    result.entry_->Commit(commit_ts);
}
void WalCmdCreateIndex::Replay(Storage *storage, u64 txn_id, u64 commit_ts) {
    auto db_entry_result = NewCatalog::GetDatabase(storage->catalog(), db_name_, txn_id, commit_ts);
    if (!db_entry_result.Success()) {
        StorageError("Wal Replay: Get database failed");
    }
    auto db_entry = dynamic_cast<DBEntry *>(db_entry_result.entry_);
    auto table_entry_result = DBEntry::GetTableCollection(db_entry, table_name_, txn_id, commit_ts);
    if (!table_entry_result.Success()) {
        StorageError("Wal Replay: Get table failed");
    }
    auto table_entry = dynamic_cast<TableCollectionEntry *>(table_entry_result.entry_);
    auto result = TableCollectionEntry::CreateIndex(table_entry, index_def_, conflict_type_, txn_id, commit_ts, nullptr);
    if (!result.Success()) {
        StorageError("Wal Replay: Create index failed");
    }
    result.entry_->Commit(commit_ts);
}
void WalCmdDropDatabase::Replay(Storage *storage, u64 txn_id, u64 commit_ts) {
    auto result = NewCatalog::DropDatabase(storage->catalog(), db_name, txn_id, commit_ts, nullptr);
    if (!result.Success()) {
        StorageError("Wal Replay: Drop database failed");
    }
    result.entry_->Commit(commit_ts);
}
void WalCmdDropTable::Replay(Storage *storage, u64 txn_id, u64 commit_ts) {
    auto db_entry_result = NewCatalog::GetDatabase(storage->catalog(), db_name, txn_id, commit_ts);
    if (!db_entry_result.Success()) {
        StorageError("Wal Replay: Get database failed");
    }
    auto db_entry = dynamic_cast<DBEntry *>(db_entry_result.entry_);
    auto result = DBEntry::DropTableCollection(db_entry, table_name, conflict_type_, txn_id, commit_ts, nullptr);
    if (!result.Success()) {
        StorageError("Wal Replay: Drop table failed");
    }
    result.entry_->Commit(commit_ts);
}
void WalCmdDelete::Replay(Storage *storage, u64 txn_id, u64 commit_ts) { NotImplementError("WalCmdDelete Replay Not implemented"); }
void WalCmdImport::Replay(Storage *storage, u64 txn_id, u64 commit_ts) { NotImplementError("WalCmdImport Replay Not implemented"); }
void WalCmdAppend::Replay(Storage *storage, u64 txn_id, u64 commit_ts) {
    auto db_entry_result = NewCatalog::GetDatabase(storage->catalog(), db_name, txn_id, commit_ts);
    if (!db_entry_result.Success()) {
        StorageError("Wal Replay: Get database failed");
    }
    auto db_entry = dynamic_cast<DBEntry *>(db_entry_result.entry_);
    auto table_entry_result = DBEntry::GetTableCollection(db_entry, table_name, txn_id, commit_ts);
    if (!table_entry_result.Success()) {
        StorageError("Wal Replay: Get table failed");
    }
    auto table_entry = dynamic_cast<TableCollectionEntry *>(table_entry_result.entry_);

    auto fake_txn = MakeUnique<Txn>(storage->txn_manager(), storage->catalog(), txn_id);

    auto table_store = MakeShared<TxnTableStore>(table_name, table_entry, fake_txn.get());
    table_store->Append(block);

    auto append_state = MakeUnique<AppendState>(table_store->blocks_);
    table_store->append_state_ = std::move(append_state);

    TableCollectionEntry::Append(table_store->table_entry_, table_store->txn_, table_store.get(), storage->buffer_manager());
    TableCollectionEntry::CommitAppend(table_store->table_entry_, table_store->txn_, table_store->append_state_.get());
}

bool WalEntry::operator==(const WalEntry &other) const {
    if (this->txn_id != other.txn_id || this->commit_ts != other.commit_ts || this->cmds.size() != other.cmds.size()) {
        return false;
    }
    for (i32 i = 0; i < this->cmds.size(); i++) {
        const SharedPtr<WalCmd> &cmd1 = this->cmds[i];
        const SharedPtr<WalCmd> &cmd2 = other.cmds[i];
        if (cmd1 == nullptr || cmd2 == nullptr || (*cmd1).operator!=(*cmd2)) {
            return false;
        }
    }
    return true;
}

bool WalEntry::operator!=(const WalEntry &other) const { return !operator==(other); }

i32 WalEntry::GetSizeInBytes() const {
    i32 size = sizeof(WalEntryHeader) + sizeof(i32);
    for (const auto &cmd : cmds) {
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
    memcpy(ptr, this, sizeof(WalEntryHeader));
    ptr += sizeof(WalEntryHeader);
    WriteBufAdv(ptr, static_cast<i32>(cmds.size()));
    for (const auto &cmd : cmds) {
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
    StorageAssert(max_bytes > 0, "ptr goes out of range when reading WalEntry");
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
    for (size_t i = 0; i < cnt; i++) {
        max_bytes = ptr_end - ptr;
        StorageAssert(max_bytes > 0, "ptr goes out of range when reading WalEntry");
        SharedPtr<WalCmd> cmd = WalCmd::ReadAdv(ptr, max_bytes);
        entry->cmds.push_back(cmd);
    }
    ptr += sizeof(i32);
    max_bytes = ptr_end - ptr;
    StorageAssert(max_bytes >= 0, "ptr goes out of range when reading WalEntry");
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
bool WalEntry::ISCheckPoint() const {
    for (const auto &cmd : cmds) {
        if (cmd->GetType() == WalCommandType::CHECKPOINT) {
            return true;
        }
    }
    return false;
}

void WalEntryIterator::Init() {
    std::ifstream ifs(wal_.c_str(), std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        StorageError("Wal open failed");
    }
    wal_size_ = ifs.tellg();
    Vector<char> buf(wal_size_);
    ifs.seekg(0, std::ios::beg);
    ifs.read(buf.data(), wal_size_);
    ifs.close();
    ptr_ = buf.data() + wal_size_;
    while (ptr_ > buf.data()) {
        i32 entry_size;
        memcpy(&entry_size, ptr_ - sizeof(i32), sizeof(entry_size));
        ptr_ = ptr_ - entry_size;
        auto entry = WalEntry::ReadAdv(ptr_, entry_size);
        ptr_ = ptr_ - entry_size;
        entries_.push_back(entry);
    }
}

bool WalEntryIterator::Next() {
    if (entry_index_ < entries_.size()) {
        return true;
    }
    return false;
}

SharedPtr<WalEntry> WalEntryIterator::GetEntry() { return entries_[entry_index_++]; }

} // namespace infinity

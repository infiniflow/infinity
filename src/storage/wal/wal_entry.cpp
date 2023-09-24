#include "wal_entry.h"
#include "common/utility/crc.hpp"
#include "common/utility/serializable.h"
#include "storage/data_block.h"
#include "storage/table_def.h"
#include <concepts>
#include <cstdint>
#include <cstring>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>

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
    case WalCommandType::INSERT_DATABLOCK: {
        String db_name = ReadBufAdv<String>(ptr);
        String table_name = ReadBufAdv<String>(ptr);
        auto cmd2 = MakeShared<WalCmdInsertDataBlock>(db_name, table_name);
        int32_t cnt = ReadBufAdv<int32_t>(ptr);
        for (int32_t i = 0; i < cnt; ++i) {
            int32_t maxbytes2 = maxbytes;
            StorageAssert(maxbytes2 > 0,
                          "buffer is exhausted when reading WalEntry");
            SharedPtr<DataBlock> block = block->ReadAdv(ptr, maxbytes2);
            cmd2->blocks.push_back(block);
        }
        cmd = cmd2;
        break;
    }
    case WalCommandType::DELETE_ROWS: {
        String db_name = ReadBufAdv<String>(ptr);
        String table_name = ReadBufAdv<String>(ptr);
        auto cmd2 = MakeShared<WalCmdDeleteRows>(db_name, table_name);
        int32_t cnt = ReadBufAdv<int32_t>(ptr);
        try {
            for (int32_t i = 0; i < cnt; ++i) {
                u64 segment_id = ReadBufAdv<u64>(ptr);
                int32_t bm_size = ReadBufAdv<int32_t>(ptr);
                StorageAssert(maxbytes > 0,
                              "buffer is exhausted when reading WalEntry");
                SharedPtr<roaring::Roaring> bm =
                    std::make_shared<roaring::Roaring>();
                roaring::Roaring r = roaring::Roaring::read(ptr);
                bm->swap(r);
                ptr += bm_size;
                cmd2->deleted_rows.insert(std::make_pair(segment_id, bm));
            }
            cmd = cmd2;
        } catch (const std::exception &e) {
            StorageError(
                std::format("roaring::Roaring::read failed. {}", e.what()));
        }
        break;
    }
    case WalCommandType::CHECKPOINT: {
        int64_t max_lsn = ReadBufAdv<int64_t>(ptr);
        cmd = MakeShared<WalCmdCheckpoint>(max_lsn);
        break;
    }
    default:
        StorageError(
            std::format("UNIMPLEMENTED ReadAdv for command {}", int(cmd_type)));
    }
    StorageAssert(ptr <= ptr_end,
                  "ptr goes out of range when reading DataBlock");
    return cmd;
}

bool WalCmdCreateTable::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCreateTable *>(&other);
    return other_cmd != nullptr && db_name == other_cmd->db_name &&
           table_def != nullptr && other_cmd->table_def != nullptr &&
           *table_def == *other_cmd->table_def;
}

bool WalCmdInsertDataBlock::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdInsertDataBlock *>(&other);
    if (other_cmd == nullptr || db_name != other_cmd->db_name ||
        table_name != other_cmd->table_name ||
        blocks.size() != other_cmd->blocks.size())
        return false;
    for (int32_t i = 0; i < blocks.size(); i++) {
        if (blocks[i] == nullptr || other_cmd->blocks[i] == nullptr)
            return false;
    }
    return true;
}

bool WalCmdDeleteRows::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdDeleteRows *>(&other);
    if (other_cmd == nullptr || db_name != other_cmd->db_name ||
        table_name != other_cmd->table_name ||
        deleted_rows.size() != other_cmd->deleted_rows.size())
        return false;
    auto it1 = deleted_rows.begin();
    auto it2 = other_cmd->deleted_rows.begin();
    for (size_t i = 0; i < deleted_rows.size(); i++) {
        u64 segment_id1 = it1->first;
        u64 segment_id2 = it2->first;
        const SharedPtr<roaring::Roaring> &r1 = it1->second;
        const SharedPtr<roaring::Roaring> &r2 = it2->second;
        if (*r1 != *r2)
            return false;
    }
    return true;
}

bool WalCmdCheckpoint::operator==(const WalCmd &other) const {
    auto other_cmd = dynamic_cast<const WalCmdCheckpoint *>(&other);
    return other_cmd != nullptr && max_lsn == other_cmd->max_lsn;
}

int32_t WalCmdCreateDatabase::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size();
}

int32_t WalCmdDropDatabase::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size();
}

int32_t WalCmdCreateTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size() +
           this->table_def->GetSizeInBytes();
}

int32_t WalCmdDropTable::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int32_t) + this->db_name.size() +
           sizeof(int32_t) + this->table_name.size();
}

int32_t WalCmdInsertDataBlock::GetSizeInBytes() const {
    int32_t size = sizeof(WalCommandType) + sizeof(int32_t) +
                   this->db_name.size() + sizeof(int32_t) +
                   this->table_name.size() + sizeof(int32_t);
    for (const auto &block : this->blocks) {
        size += block->GetSizeInBytes();
    }
    return size;
}

int32_t WalCmdDeleteRows::GetSizeInBytes() const {
    int32_t size = sizeof(WalCommandType) + sizeof(int32_t) +
                   this->db_name.size() + sizeof(int32_t) +
                   this->table_name.size() + sizeof(int32_t);
    for (const auto &item : this->deleted_rows) {
        size += sizeof(u64) + sizeof(int32_t) + item.second->getSizeInBytes();
    }
    return size;
}

int32_t WalCmdCheckpoint::GetSizeInBytes() const {
    return sizeof(WalCommandType) + sizeof(int64_t);
}

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

void WalCmdDropTable::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DROP_TABLE);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
}

void WalCmdInsertDataBlock::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::INSERT_DATABLOCK);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
    WriteBufAdv(buf, static_cast<int32_t>(this->blocks.size()));
    for (const auto &block : this->blocks) {
        block->WriteAdv(buf);
    }
}

void WalCmdDeleteRows::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::DELETE_ROWS);
    WriteBufAdv(buf, this->db_name);
    WriteBufAdv(buf, this->table_name);
    WriteBufAdv(buf, static_cast<int32_t>(this->deleted_rows.size()));
    for (const auto &item : this->deleted_rows) {
        WriteBufAdv(buf, item.first); // segment id
        const SharedPtr<roaring::Roaring> &r = item.second;
        r->write(buf);
    }
}

void WalCmdCheckpoint::WriteAdv(char *&buf) const {
    WriteBufAdv(buf, WalCommandType::CHECKPOINT);
    WriteBufAdv(buf, this->max_lsn);
}

bool WalEntry::operator==(const WalEntry &other) const {
    if (this->lsn != other.lsn || this->txn_id != other.txn_id ||
        this->cmds.size() != other.cmds.size()) {
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

bool WalEntry::operator!=(const WalEntry &other) const {
    return !operator==(other);
}

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
    header->checksum = CRC32IEEE::makeCRC(
        reinterpret_cast<const unsigned char *>(saved_ptr), size);
    return;
}

SharedPtr<WalEntry> WalEntry::ReadAdv(char *&ptr, int32_t maxbytes) {
    char *const ptr_end = ptr + maxbytes;
    StorageAssert(maxbytes > 0, "buffer is exhausted when reading WalEntry");
    SharedPtr<WalEntry> entry = MakeShared<WalEntry>();
    WalEntryHeader *header = (WalEntryHeader *)ptr;
    entry->lsn = header->lsn;
    entry->size = header->size;
    entry->checksum = header->checksum;
    entry->txn_id = header->txn_id;
    int32_t size2 = *(int32_t *)(ptr + entry->size - sizeof(int32_t));
    if (entry->size != size2) {
        return nullptr;
    }
    header->checksum = 0;
    uint32_t checksum2 = CRC32IEEE::makeCRC(
        reinterpret_cast<const unsigned char *>(ptr), entry->size);
    if (entry->checksum != checksum2) {
        return nullptr;
    }
    ptr += sizeof(WalEntryHeader);
    int32_t cnt = ReadBufAdv<int32_t>(ptr);
    for (size_t i = 0; i < cnt; i++) {
        maxbytes = ptr_end - ptr;
        StorageAssert(maxbytes > 0,
                      "buffer is exhausted when reading WalEntry");
        SharedPtr<WalCmd> cmd = WalCmd::ReadAdv(ptr, maxbytes);
        entry->cmds.push_back(cmd);
    }
    ptr += sizeof(int32_t);
    StorageAssert(ptr <= ptr_end, "buffer is exhausted when reading WalEntry");
    return entry;
}

} // namespace infinity

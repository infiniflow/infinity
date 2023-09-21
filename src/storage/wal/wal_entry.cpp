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

bool WalEntry::operator==(const WalEntry &other) const {
    if (this->lsn != other.lsn || this->txn_id != other.txn_id ||
        this->dropped_databases_ != other.dropped_databases_ ||
        this->dropped_tables_ != other.dropped_tables_ ||
        this->created_databases_ != other.created_databases_ ||
        this->created_tables_.size() != other.created_tables_.size()) {
        return false;
    }
    for (int32_t i = 0; i < this->created_tables_.size(); i++) {
        const String &db_name1 = this->created_tables_[i].first;
        const String &db_name2 = other.created_tables_[i].first;
        if (db_name1 != db_name2) {
            return false;
        }
        const SharedPtr<TableDef> &table_def1 = this->created_tables_[i].second;
        const SharedPtr<TableDef> &table_def2 = other.created_tables_[i].second;
        if (table_def1 == nullptr || table_def2 == nullptr ||
            *table_def1 != *table_def2) {
            return false;
        }
    }
    return true;
}

bool WalEntry::operator!=(const WalEntry &other) const {
    return !operator==(other);
}

int32_t WalEntry::GetSizeInBytes() const {
    int32_t size = sizeof(WalEntryHeader);
    if (!dropped_databases_.empty()) {
        size++;                  // WalCommandType::DROP_DATABASE
        size += sizeof(int32_t); // dropped_databases_.size()
        for (const auto &db_name : dropped_databases_) {
            size += sizeof(int32_t); // db_name.size()
            size += db_name.size();
        }
    }
    if (!dropped_tables_.empty()) {
        size++;                  // WalCommandType::DROP_TABLE
        size += sizeof(int32_t); // dropped_tables_.size()
        for (const auto &db_tbl : dropped_tables_) {
            size += sizeof(int32_t); // db_tbl.first.size()
            size += db_tbl.first.size();
            size += sizeof(int32_t); // db_tbl.second.size()
            size += db_tbl.second.size();
        }
    }
    if (!created_databases_.empty()) {
        size++;                  // WalCommandType::CREATE_DATABASE
        size += sizeof(int32_t); // created_databases_.size()
        for (const auto &db_name : created_databases_) {
            size += sizeof(int32_t); // db_name.size()
            size += db_name.size();
        }
    }
    if (!created_tables_.empty()) {
        size++;                  // WalCommandType::CREATE_TABLE
        size += sizeof(int32_t); // created_tables_.size()
        for (const auto &item : created_tables_) {
            size += sizeof(int32_t); // db_name.size()
            size += item.first.size();
            size += item.second->GetSizeInBytes();
        }
    }
    if (!deleted_rows_.empty()) {
        size++;                  // WalCommandType::DELETE_ROWS
        size += sizeof(int32_t); // deleted_rows_.size()
        for (const auto &item : deleted_rows_) {
            size += sizeof(int64_t); // item.first
            size += sizeof(int32_t); // bitmap size
            size += item.second->getSizeInBytes();
        }
    }
    if (!blocks_.empty()) {
        size++;                  // WalCommandType::INSERT_DATABLOCK
        size += sizeof(int32_t); // blocks_.size()
        for (const auto &block : blocks_) {
            size += sizeof(int32_t); // block->GetSizeInBytes()
            size += block->GetSizeInBytes();
        }
    }
    if (max_lsn_ > 0) {
        size++;                  // WalCommandType::CHECKPOINT
        size += sizeof(int64_t); // max_lsn_
    }
    size += sizeof(int32_t); // pad
    return size;
}

void WalEntry::WriteAdv(char *&ptr) const {
    // An entry is serialized as follows:
    // - WalEntryHeader
    // - (optional) TLV of dropped_databases_
    // - (optional) TLV of dropped_tables_
    // - (optional) TLV of created_databases_
    // - (optional) TLV of created_tables_
    // - (optional) TLV of deleted_rows_
    // - (optional) TLV of blocks_
    // - (optional) TLV of max_lsn_
    // - 4 bytes pad
    char *saved_ptr = ptr;
    memcpy(ptr, this, sizeof(WalEntryHeader));
    ptr += sizeof(WalEntryHeader);
    if (!dropped_databases_.empty()) {
        WriteBufAdv(ptr, WalCommandType::DROP_DATABASE);
        WriteBufAdv(ptr, static_cast<int32_t>(dropped_databases_.size()));
        for (const auto &db_name : dropped_databases_) {
            WriteBufAdv(ptr, db_name);
        }
    }
    if (!dropped_tables_.empty()) {
        WriteBufAdv(ptr, WalCommandType::DROP_TABLE);
        WriteBufAdv(ptr, static_cast<int32_t>(dropped_tables_.size()));
        for (const auto &db_tbl : dropped_tables_) {
            WriteBufAdv(ptr, db_tbl.first);
            WriteBufAdv(ptr, db_tbl.second);
        }
    }
    if (!created_databases_.empty()) {
        WriteBufAdv(ptr, WalCommandType::CREATE_DATABASE);
        WriteBufAdv(ptr, static_cast<int32_t>(created_databases_.size()));
        for (const auto &db_name : created_databases_) {
            WriteBufAdv(ptr, db_name);
        }
    }
    if (!created_tables_.empty()) {
        WriteBufAdv(ptr, WalCommandType::CREATE_TABLE);
        WriteBufAdv(ptr, static_cast<int32_t>(created_tables_.size()));
        for (const auto &item : created_tables_) {
            WriteBufAdv(ptr, item.first);
            item.second->WriteAdv(ptr);
        }
    }
    if (!deleted_rows_.empty()) {
        WriteBufAdv(ptr, WalCommandType::DELETE_ROWS);
        WriteBufAdv(ptr, static_cast<int32_t>(deleted_rows_.size()));
        for (const auto &item : deleted_rows_) {
            WriteBufAdv(ptr, (u64)(item.first));
            int32_t bm_size = item.second->getSizeInBytes();
            WriteBufAdv(ptr, bm_size);
            bm_size = item.second->write(ptr);
            ptr += bm_size;
        }
    }
    if (!blocks_.empty()) {
        WriteBufAdv(ptr, WalCommandType::INSERT_DATABLOCK);
        WriteBufAdv(ptr, static_cast<int32_t>(blocks_.size()));
        for (const auto &block : blocks_) {
            block->WriteAdv(ptr);
        }
    }
    if (max_lsn_ > 0) {
        WriteBufAdv(ptr, WalCommandType::CHECKPOINT);
        WriteBufAdv(ptr, static_cast<int64_t>(max_lsn_));
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
    char *const saved_ptr = ptr;
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
    char *end = ptr + entry->size - sizeof(int32_t);
    ptr += sizeof(WalEntryHeader);
    while (ptr < end) {
        WalCommandType cmd_type = ReadBufAdv<WalCommandType>(ptr);
        int32_t cnt;
        switch (cmd_type) {
        case WalCommandType::CREATE_DATABASE: {
            String db_name = ReadBufAdv<String>(ptr);
            entry->created_databases_.push_back(db_name);
            break;
        }
        case WalCommandType::CREATE_TABLE: {
            cnt = ReadBufAdv<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                String db_name = ReadBufAdv<String>(ptr);
                int32_t maxbytes2 = maxbytes;
                StorageAssert(maxbytes2 > 0,
                              "buffer is exhausted when reading WalEntry");
                SharedPtr<TableDef> table_def =
                    TableDef::ReadAdv(ptr, maxbytes2);
                entry->created_tables_.push_back(std::pair(db_name, table_def));
            }
            break;
        }
        case WalCommandType::ALTER_INFO:
            return nullptr;
        case WalCommandType::DROP_DATABASE:
            cnt = ReadBufAdv<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                String db_name = ReadBufAdv<String>(ptr);
                entry->dropped_databases_.push_back(db_name);
            }
            break;
        case WalCommandType::DROP_TABLE:
            cnt = ReadBufAdv<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                String db_name = ReadBufAdv<String>(ptr);
                String tbl_name = ReadBufAdv<String>(ptr);
                entry->dropped_tables_.push_back(
                    std::make_pair(db_name, tbl_name));
            }
            break;
        case WalCommandType::DELETE_ROWS:
            cnt = ReadBufAdv<int32_t>(ptr);
            try {
                for (int32_t i = 0; i < cnt; ++i) {
                    u64 block_id = ReadBufAdv<u64>(ptr);
                    int32_t bm_size = ReadBufAdv<int32_t>(ptr);
                    StorageAssert(maxbytes > 0,
                                  "buffer is exhausted when reading WalEntry");
                    SharedPtr<roaring::Roaring> bm =
                        std::make_shared<roaring::Roaring>();
                    roaring::Roaring r = roaring::Roaring::read(ptr);
                    bm->swap(r);
                    ptr += bm_size;
                    entry->deleted_rows_.insert(std::make_pair(block_id, bm));
                }
            } catch (const std::exception &e) {
                StorageError(
                    std::format("roaring::Roaring::read failed. {}", e.what()));
            }
            break;
        case WalCommandType::INSERT_DATABLOCK:
            cnt = ReadBufAdv<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                int32_t maxbytes2 = saved_ptr + maxbytes - ptr;
                StorageAssert(maxbytes2 > 0,
                              "buffer is exhausted when reading WalEntry");
                SharedPtr<DataBlock> block = block->ReadAdv(ptr);
                entry->blocks_.push_back(block);
            }
            break;
        default:
            return nullptr;
        }
    }
    ptr += sizeof(int32_t);
    return entry;
}

} // namespace infinity

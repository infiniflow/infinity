#include "wal_entry.h"
#include "common/utility/crc.hpp"
#include <concepts>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <format>
#include <type_traits>

namespace infinity {

template <typename T>
concept POD = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

template <typename T> inline T ReadBuf(char *const buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    T value = *ptr;
    return value;
}

template <typename T> inline T ReadBufAdvance(char *&buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    T value = *ptr;
    buf += sizeof(T);
    return value;
}

template <>
inline String ReadBuf<String>(char *const buf) {
    int32_t size = ReadBuf<int32_t>(buf);
    String str(buf + sizeof(int32_t), size);
    return str;
}

template <>
inline String ReadBufAdvance<String>(char *&buf) {
    int32_t size = ReadBufAdvance<int32_t>(buf);
    String str(buf, size);
    buf += size;
    return str;
}

template <typename T> inline void WriteBuf(char *const buf, const T& value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    *ptr = value;
}

template <typename T> inline void WriteBufAdvance(char *&buf, const T& value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    *ptr = value;
    buf += sizeof(T);
}

template <>
inline void WriteBuf<String>(char *const buf, const String &value) {
    int32_t len = value.length();
    WriteBuf(buf, len);
    memcpy(buf + len, value.c_str(), len);
}

template <>
inline void WriteBufAdvance<String>(char *&buf, const String &value) {
    int32_t len = value.length();
    WriteBufAdvance(buf, len);
    memcpy(buf, value.c_str(), len);
    buf += len;
}

int32_t WALEntry::GetSizeInBytes() const {
    int32_t size = sizeof(WALEntryHeader);
    if (!dropped_databases_.empty()) {
        size++;                  // WALCommandType::DROP_DATABASE
        size += sizeof(int32_t); // dropped_databases_.size()
        for (const auto &db_name : dropped_databases_) {
            size += sizeof(int32_t); // db_name.size()
            size += db_name.size();
        }
    }
    if (!dropped_tables_.empty()) {
        size++;                  // WALCommandType::DROP_TABLE
        size += sizeof(int32_t); // dropped_tables_.size()
        for (const auto &db_tbl : dropped_tables_) {
            size += sizeof(int32_t); // db_tbl.first.size()
            size += db_tbl.first.size();
            size += sizeof(int32_t); // db_tbl.second.size()
            size += db_tbl.second.size();
        }
    }
    if (!deleted_rows_.empty()) {
        size++;                  // WALCommandType::DELETE_ROWS
        size += sizeof(int32_t); // deleted_rows_.size()
        for (const auto &item : deleted_rows_) {
            size += sizeof(int64_t); // item.first
            size += item.second->getSizeInBytes();
        }
    }
    if (!blocks_.empty()) {
        size++;                  // WALCommandType::INSERT_DATABLOCK
        size += sizeof(int32_t); // blocks_.size()
        for (const auto &block : blocks_) {
            size += sizeof(int32_t); // block->GetSizeInBytes()
            size += block->GetSizeInBytes();
        }
    }
    if (max_lsn_ > 0) {
        size++;                  // WALCommandType::CHECKPOINT
        size += sizeof(int64_t); // max_lsn_
    }
    size += sizeof(int32_t); // pad
    return size;
}

int32_t WALEntry::Write(char *buf, int32_t maxbytes) const {
    // An entry is serialized as follows:
    // - WALEntryHeader
    // - (optional) TLV of dropped_databases_
    // - (optional) TLV of dropped_tables_
    // - (optional) TLV of deleted_rows_
    // - (optional) TLV of blocks_
    // - (optional) TLV of max_lsn_
    // - 4 bytes pad
    char *ptr = buf;
    memcpy(ptr, this, sizeof(WALEntryHeader));
    ptr += sizeof(WALEntryHeader);
    if (!dropped_databases_.empty()) {
        WriteBufAdvance(ptr, WALCommandType::DROP_DATABASE);
        WriteBufAdvance(ptr, static_cast<int32_t>(dropped_databases_.size()));
        for (const auto &db_name : dropped_databases_) {
            WriteBufAdvance(ptr, db_name);
        }
    }
    if (!dropped_tables_.empty()) {
        WriteBufAdvance(ptr, WALCommandType::DROP_TABLE);
        WriteBufAdvance(ptr, static_cast<int32_t>(dropped_tables_.size()));
        for (const auto &db_tbl : dropped_tables_) {
            WriteBufAdvance(ptr, db_tbl.first);
            WriteBufAdvance(ptr, db_tbl.second);
        }
    }
    if (!deleted_rows_.empty()) {
        WriteBufAdvance(ptr, WALCommandType::DELETE_ROWS);
        WriteBufAdvance(ptr, static_cast<int32_t>(deleted_rows_.size()));
        for (const auto &item : deleted_rows_) {
            WriteBufAdvance(ptr, (u64)(item.first));
            int32_t bm_size = item.second->getSizeInBytes();
            WriteBufAdvance(ptr, bm_size);
            bm_size = item.second->write(ptr);
            ptr += bm_size;
        }
    }
    if (!blocks_.empty()) {
        WriteBufAdvance(ptr, WALCommandType::INSERT_DATABLOCK);
        WriteBufAdvance(ptr, static_cast<int32_t>(blocks_.size()));
        for (const auto &block : blocks_) {
            int32_t block_size = block->GetSizeInBytes();
            WriteBufAdvance(ptr, block_size);
            block_size = block->Write(ptr, block_size);
            ptr += block_size;
        }
    }
    if (max_lsn_ > 0) {
        WriteBufAdvance(ptr, WALCommandType::CHECKPOINT);
        WriteBufAdvance(ptr, static_cast<int64_t>(max_lsn_));
    }
    int32_t size = ptr - buf;
    WALEntryHeader *header = (WALEntryHeader *)buf;
    header->size = size;
    header->checksum = 0;
    // CRC32IEEE is equivalent to boost::crc_32_type on
    // little-endian machine.
    header->checksum =
        CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(buf), size);
    WriteBuf(ptr, size);
    return ptr - buf;
}

int32_t WALEntry::Read(char *buf, int32_t maxbytes, WALEntry &entry) {
    WALEntryHeader *header = (WALEntryHeader *)buf;
    entry.lsn = header->lsn;
    entry.size = header->size;
    entry.checksum = header->checksum;
    entry.txn_id = header->txn_id;
    if (entry.size > maxbytes) {
        return -1;
    }
    int32_t size2 = *(int32_t *)(buf + entry.size - sizeof(int32_t));
    if (entry.size != size2) {
        return -1;
    }
    header->checksum = 0;
    uint32_t checksum2 = CRC32IEEE::makeCRC(
        reinterpret_cast<const unsigned char *>(buf), entry.size);
    if (entry.checksum != checksum2) {
        return -1;
    }
    char *ptr = buf + sizeof(WALEntryHeader);
    char *end = buf + entry.size - sizeof(int32_t);
    while (ptr < end) {
        WALCommandType cmd_type = ReadBufAdvance<WALCommandType>(ptr);
        int32_t cnt;
        switch (cmd_type) {
        case WALCommandType::CREATE_DATABASE:
            return -1;
        case WALCommandType::CREATE_TABLE:
            return -1;
        case WALCommandType::ALTER_INFO:
            return -1;
        case WALCommandType::DROP_DATABASE:
            cnt = ReadBufAdvance<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                String db_name = ReadBufAdvance<String>(ptr);
                entry.dropped_databases_.push_back(db_name);
            }
            break;
        case WALCommandType::DROP_TABLE:
            cnt = ReadBufAdvance<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                String db_name = ReadBufAdvance<String>(ptr);
                String tbl_name = ReadBufAdvance<String>(ptr);
                entry.dropped_tables_.push_back(
                    std::make_pair(db_name, tbl_name));
            }
            break;
        case WALCommandType::DELETE_ROWS:
            cnt = ReadBufAdvance<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                u64 block_id = ReadBufAdvance<u64>(ptr);
                int32_t bm_size = ReadBufAdvance<int32_t>(ptr);
                SharedPtr<roaring::Roaring> bm =
                    std::make_shared<roaring::Roaring>();
                roaring::Roaring r = roaring::Roaring::read(ptr);
                bm->swap(r);
                ptr += bm_size;
                entry.deleted_rows_.insert(std::make_pair(block_id, bm));
            }
            break;
        case WALCommandType::INSERT_DATABLOCK:
            cnt = ReadBufAdvance<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                int32_t block_size = ReadBufAdvance<int32_t>(ptr);
                SharedPtr<DataBlock> block = MakeShared<DataBlock>();
                int32_t actual_size = block->Read(ptr, block_size, *block);
                if (actual_size < 0)
                    return -1;
                ptr += actual_size;
                entry.blocks_.push_back(block);
            }
            break;
        default:
            return -1;
        }
    }
    return (ptr == end) ? 0 : -1;
}

} // namespace infinity

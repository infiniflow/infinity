#include "wal_entry.h"
#include "storage/data_block.h"
#include "common/utility/crc.hpp"
#include "storage/table_def.h"
#include <cstdint>
#include <cstring>
#include <string>
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
            size += sizeof(int32_t); // item.second
            size += item.second->GetSizeInBytes();
        }
    }
    if (!deleted_rows_.empty()) {
        size++;                  // WalCommandType::DELETE_ROWS
        size += sizeof(int32_t); // deleted_rows_.size()
        for (const auto &item : deleted_rows_) {
            size += sizeof(int64_t); // item.first
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

int32_t WalEntry::Write(char *buf, int32_t maxbytes) const {
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
    char *ptr = buf;
    memcpy(ptr, this, sizeof(WalEntryHeader));
    ptr += sizeof(WalEntryHeader);
    if (!dropped_databases_.empty()) {
        WriteBufAdvance(ptr, WalCommandType::DROP_DATABASE);
        WriteBufAdvance(ptr, static_cast<int32_t>(dropped_databases_.size()));
        for (const auto &db_name : dropped_databases_) {
            WriteBufAdvance(ptr, db_name);
        }
    }
    if (!dropped_tables_.empty()) {
        WriteBufAdvance(ptr, WalCommandType::DROP_TABLE);
        WriteBufAdvance(ptr, static_cast<int32_t>(dropped_tables_.size()));
        for (const auto &db_tbl : dropped_tables_) {
            WriteBufAdvance(ptr, db_tbl.first);
            WriteBufAdvance(ptr, db_tbl.second);
        }
    }
    if (!created_databases_.empty()) {
        WriteBufAdvance(ptr, WalCommandType::CREATE_DATABASE);
        WriteBufAdvance(ptr, static_cast<int32_t>(created_databases_.size()));
        for (const auto &db_name : created_databases_) {
            WriteBufAdvance(ptr, db_name);
        }
    }
    if (!created_tables_.empty()) {
        WriteBufAdvance(ptr, WalCommandType::CREATE_TABLE);
        WriteBufAdvance(ptr, static_cast<int32_t>(created_tables_.size()));
        for (const auto &item : created_tables_) {
            WriteBufAdvance(ptr, item.first);
            int32_t size = item.second->GetSizeInBytes();
            WriteBufAdvance(ptr, size);
            size = item.second->Write(ptr, size);
            ptr += size;
        }
    }
    if (!deleted_rows_.empty()) {
        WriteBufAdvance(ptr, WalCommandType::DELETE_ROWS);
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
        WriteBufAdvance(ptr, WalCommandType::INSERT_DATABLOCK);
        WriteBufAdvance(ptr, static_cast<int32_t>(blocks_.size()));
        for (const auto &block : blocks_) {
            int32_t block_size = block->GetSizeInBytes();
            WriteBufAdvance(ptr, block_size);
            block_size = block->Write(ptr, block_size);
            ptr += block_size;
        }
    }
    if (max_lsn_ > 0) {
        WriteBufAdvance(ptr, WalCommandType::CHECKPOINT);
        WriteBufAdvance(ptr, static_cast<int64_t>(max_lsn_));
    }
    int32_t size = ptr - buf;
    WalEntryHeader *header = (WalEntryHeader *)buf;
    header->size = size;
    header->checksum = 0;
    // CRC32IEEE is equivalent to boost::crc_32_type on
    // little-endian machine.
    header->checksum =
        CRC32IEEE::makeCRC(reinterpret_cast<const unsigned char *>(buf), size);
    WriteBuf(ptr, size);
    return ptr - buf;
}

int32_t WalEntry::Read(char *buf, int32_t maxbytes, WalEntry &entry) {
    WalEntryHeader *header = (WalEntryHeader *)buf;
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
    char *ptr = buf + sizeof(WalEntryHeader);
    char *end = buf + entry.size - sizeof(int32_t);
    while (ptr < end) {
        WalCommandType cmd_type = ReadBufAdvance<WalCommandType>(ptr);
        int32_t cnt;
        switch (cmd_type) {
        case WalCommandType::CREATE_DATABASE:
            return -1;
        case WalCommandType::CREATE_TABLE:
            return -1;
        case WalCommandType::ALTER_INFO:
            return -1;
        case WalCommandType::DROP_DATABASE:
            cnt = ReadBufAdvance<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                String db_name = ReadBufAdvance<String>(ptr);
                entry.dropped_databases_.push_back(db_name);
            }
            break;
        case WalCommandType::DROP_TABLE:
            cnt = ReadBufAdvance<int32_t>(ptr);
            for (int32_t i = 0; i < cnt; ++i) {
                String db_name = ReadBufAdvance<String>(ptr);
                String tbl_name = ReadBufAdvance<String>(ptr);
                entry.dropped_tables_.push_back(
                    std::make_pair(db_name, tbl_name));
            }
            break;
        case WalCommandType::DELETE_ROWS:
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
        case WalCommandType::INSERT_DATABLOCK:
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

#pragma once

#include "common/types/internal_types.h"
#include <cstdint>
#include <roaring/roaring.hh>
namespace infinity {

class TableDef;
class DataBlock;

enum class WalCommandType : uint8_t {
    INVALID = 0,
    // -----------------------------
    // Catalog
    // -----------------------------
    CREATE_DATABASE = 1,
    DROP_DATABASE = 2,
    CREATE_TABLE = 3,
    DROP_TABLE = 4,
    ALTER_INFO = 5,

    // -----------------------------
    // Data
    // -----------------------------
    APPEND = 21,
    DELETE = 22,
    // -----------------------------
    // Flush
    // -----------------------------
    CHECKPOINT = 99,
};

struct WalCmd {
    virtual ~WalCmd() = default;
    virtual WalCommandType GetType() = 0;
    virtual bool operator==(const WalCmd &other) const {
        return typeid(*this) == typeid(other);
    }
    bool operator!=(const WalCmd &other) { return !(*this == other); }
    // Estimated serialized size in bytes, ensured be no less than Write
    // requires, allowed be larger.
    virtual int32_t GetSizeInBytes() const = 0;
    // Write to a char buffer
    virtual void WriteAdv(char *&buf) const = 0;
    // Read from a serialized version
    static SharedPtr<WalCmd> ReadAdv(char *&buf, int32_t maxbytes);
};

struct WalCmdCreateDatabase : public WalCmd {
    WalCmdCreateDatabase(const String &db_name_) : db_name(db_name_) {}
    virtual WalCommandType GetType() { return WalCommandType::CREATE_DATABASE; }
    virtual bool operator==(const WalCmd &other) const {
        auto other_cmd = dynamic_cast<const WalCmdCreateDatabase *>(&other);
        return other_cmd != nullptr && db_name == other_cmd->db_name;
    }
    int32_t GetSizeInBytes() const;
    void WriteAdv(char *&buf) const;

    String db_name;
};

struct WalCmdDropDatabase : public WalCmd {
    WalCmdDropDatabase(const String &db_name_) : db_name(db_name_) {}
    virtual WalCommandType GetType() { return WalCommandType::DROP_DATABASE; }
    virtual bool operator==(const WalCmd &other) const {
        auto other_cmd = dynamic_cast<const WalCmdDropDatabase *>(&other);
        return other_cmd != nullptr && db_name == other_cmd->db_name;
    }
    virtual int32_t GetSizeInBytes() const;
    virtual void WriteAdv(char *&buf) const;
    String db_name;
};

struct WalCmdCreateTable : public WalCmd {
    WalCmdCreateTable(const String &db_name_, const SharedPtr<TableDef> &table_def_)
        : db_name(db_name_), table_def(table_def_) {}
    virtual WalCommandType GetType() { return WalCommandType::CREATE_TABLE; }
    virtual bool operator==(const WalCmd &other) const;
    virtual int32_t GetSizeInBytes() const;
    virtual void WriteAdv(char *&buf) const;
    String db_name;
    SharedPtr<TableDef> table_def;
};

struct WalCmdDropTable : public WalCmd {
    WalCmdDropTable(const String &db_name_, const String &table_name_)
        : db_name(db_name_), table_name(table_name_) {}
    virtual WalCommandType GetType() { return WalCommandType::DROP_TABLE; }
    virtual bool operator==(const WalCmd &other) const {
        auto other_cmd = dynamic_cast<const WalCmdDropTable *>(&other);
        return other_cmd != nullptr && db_name == other_cmd->db_name &&
               table_name == other_cmd->table_name;
    }
    int32_t GetSizeInBytes() const;
    void WriteAdv(char *&buf) const;
    String db_name;
    String table_name;
};

struct WalCmdAppend : public WalCmd {
    WalCmdAppend(const String &db_name_, const String &table_name_, const SharedPtr<DataBlock>& block_)
        : db_name(db_name_), table_name(table_name_), block(block_) {}
    virtual WalCommandType GetType() { return WalCommandType::APPEND; }
    virtual bool operator==(const WalCmd &other) const;
    virtual int32_t GetSizeInBytes() const;
    virtual void WriteAdv(char *&buf) const;
    String db_name;
    String table_name;
    SharedPtr<DataBlock> block;
};

struct WalCmdDelete : public WalCmd {
    WalCmdDelete(const String &db_name_, const String &table_name_, const Vector<RowID>&row_ids_)
        : db_name(db_name_), table_name(table_name_), row_ids(row_ids_) {}
    virtual WalCommandType GetType() { return WalCommandType::DELETE; }
    virtual bool operator==(const WalCmd &other) const;
    virtual int32_t GetSizeInBytes() const;
    virtual void WriteAdv(char *&buf) const;
    String db_name;
    String table_name;
    Vector<RowID> row_ids;
};

struct WalCmdCheckpoint : public WalCmd {
    WalCmdCheckpoint(int64_t max_commit_ts_) : max_commit_ts(max_commit_ts_) {}
    virtual WalCommandType GetType() { return WalCommandType::CHECKPOINT; }
    virtual bool operator==(const WalCmd &other) const;
    virtual int32_t GetSizeInBytes() const;
    virtual void WriteAdv(char *&buf) const;
    int64_t max_commit_ts;
};

struct WalEntryHeader {
    int32_t size; // size of payload, excluding the header, round to multi
                  // of 4. There's 4 bytes pad just after the payload storing
                  // the same value to assist backward iterating.
    uint32_t checksum; // crc32 of the entry, including the header and the
                       // payload. User shall populate it before writing to wal.
    int64_t txn_id; // txn id of the entry
    int64_t commit_ts; // commit timestamp of the txn
};

struct WalEntry : WalEntryHeader {
    bool operator==(const WalEntry &other) const;
    bool operator!=(const WalEntry &other) const;

    // Estimated serialized size in bytes, ensured be no less than Write
    // requires, allowed be larger.
    int32_t GetSizeInBytes() const;
    // Write to a char buffer
    void WriteAdv(char *&buf) const;
    // Read from a serialized version
    static SharedPtr<WalEntry> ReadAdv(char *&buf, int32_t maxbytes);

    Vector<SharedPtr<WalCmd>> cmds;
};

} // namespace infinity

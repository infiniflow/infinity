
module;

#include <typeinfo>

import stl;
import parser;
import data_block;
import table_def;

export module wal_entry;

namespace infinity {

//class TableDef;
//
//class DataBlock;

enum class WalCommandType : i8 {
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
    IMPORT = 20,
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

    virtual bool operator==(const WalCmd &other) const { return typeid(*this) == typeid(other); }
    bool operator!=(const WalCmd &other) { return !(*this == other); }
    // Estimated serialized size in bytes
    virtual i32 GetSizeInBytes() const = 0;
    // Write to a char buffer
    virtual void WriteAdv(char *&ptr) const = 0;
    // Read from a serialized version
    static SharedPtr<WalCmd> ReadAdv(char *&ptr, i32 maxbytes);
};

struct WalCmdCreateDatabase : public WalCmd {
    WalCmdCreateDatabase(const String &db_name_) : db_name(db_name_) {}
    virtual WalCommandType GetType() { return WalCommandType::CREATE_DATABASE; }
    virtual bool operator==(const WalCmd &other) const {
        auto other_cmd = dynamic_cast<const WalCmdCreateDatabase *>(&other);
        return other_cmd != nullptr && IsEqual(db_name, other_cmd->db_name);
    }

    i32 GetSizeInBytes() const;

    void WriteAdv(char *&buf) const;

    String db_name;
};

struct WalCmdDropDatabase : public WalCmd {
    WalCmdDropDatabase(const String &db_name_) : db_name(db_name_) {}
    virtual WalCommandType GetType() { return WalCommandType::DROP_DATABASE; }
    virtual bool operator==(const WalCmd &other) const {
        auto other_cmd = dynamic_cast<const WalCmdDropDatabase *>(&other);
        return other_cmd != nullptr && IsEqual(db_name, other_cmd->db_name);
    }

    virtual i32 GetSizeInBytes() const;

    virtual void WriteAdv(char *&buf) const;

    String db_name;
};

struct WalCmdCreateTable : public WalCmd {
    WalCmdCreateTable(const String &db_name_, const SharedPtr<TableDef> &table_def_) : db_name(db_name_), table_def(table_def_) {}
    virtual WalCommandType GetType() { return WalCommandType::CREATE_TABLE; }

    virtual bool operator==(const WalCmd &other) const;

    virtual i32 GetSizeInBytes() const;

    virtual void WriteAdv(char *&buf) const;

    String db_name;
    SharedPtr<TableDef> table_def;
};

struct WalCmdDropTable : public WalCmd {
    WalCmdDropTable(const String &db_name_, const String &table_name_) : db_name(db_name_), table_name(table_name_) {}
    virtual WalCommandType GetType() { return WalCommandType::DROP_TABLE; }
    virtual bool operator==(const WalCmd &other) const {
        auto other_cmd = dynamic_cast<const WalCmdDropTable *>(&other);
        return other_cmd != nullptr && IsEqual(db_name, other_cmd->db_name) && IsEqual(table_name, other_cmd->table_name);
    }

    i32 GetSizeInBytes() const;

    void WriteAdv(char *&buf) const;

    String db_name;
    String table_name;
};

struct WalCmdImport : public WalCmd {
    WalCmdImport(const String &db_name_, const String &table_name_, const String &segment_dir_)
        : db_name(db_name_), table_name(table_name_), segment_dir(segment_dir_) {}
    virtual WalCommandType GetType() { return WalCommandType::IMPORT; }
    virtual bool operator==(const WalCmd &other) const;
    virtual i32 GetSizeInBytes() const;
    virtual void WriteAdv(char *&buf) const;
    String db_name;
    String table_name;
    String segment_dir;
};

struct WalCmdAppend : public WalCmd {
    WalCmdAppend(const String &db_name_, const String &table_name_, const SharedPtr<DataBlock> &block_)
        : db_name(db_name_), table_name(table_name_), block(block_) {}
    virtual WalCommandType GetType() { return WalCommandType::APPEND; }

    virtual bool operator==(const WalCmd &other) const;

    virtual i32 GetSizeInBytes() const;

    virtual void WriteAdv(char *&buf) const;

    String db_name;
    String table_name;
    SharedPtr<DataBlock> block;
};

struct WalCmdDelete : public WalCmd {
    WalCmdDelete(const String &db_name_, const String &table_name_, const Vector<RowT> &row_ids_)
        : db_name(db_name_), table_name(table_name_), row_ids(row_ids_) {}
    virtual WalCommandType GetType() { return WalCommandType::DELETE; }

    virtual bool operator==(const WalCmd &other) const;

    virtual i32 GetSizeInBytes() const;

    virtual void WriteAdv(char *&buf) const;

    String db_name;
    String table_name;
    Vector<RowT> row_ids;
};

struct WalCmdCheckpoint : public WalCmd {
    WalCmdCheckpoint(i64 max_commit_ts_, String catalog_path) : max_commit_ts_(max_commit_ts_), catalog_path_(catalog_path) {}
    virtual WalCommandType GetType() { return WalCommandType::CHECKPOINT; }

    virtual bool operator==(const WalCmd &other) const;

    virtual i32 GetSizeInBytes() const;

    virtual void WriteAdv(char *&buf) const;

    i64 max_commit_ts_;
    String catalog_path_;
};

struct WalEntryHeader {
    i32 size; // size of payload, excluding the header, round to multi
    // of 4. There's 4 bytes pad just after the payload storing
    // the same value to assist backward iterating.
    u32 checksum; // crc32 of the entry, including the header and the
    // payload. User shall populate it before writing to wal.
    i64 txn_id;    // txn id of the entry
    i64 commit_ts; // commit timestamp of the txn
};

export struct WalEntry : WalEntryHeader {
    bool operator==(const WalEntry &other) const;

    bool operator!=(const WalEntry &other) const;

    // Estimated serialized size in bytes, ensured be no less than Write
    // requires, allowed be larger.
    i32 GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static SharedPtr<WalEntry> ReadAdv(char *&ptr, i32 maxbytes);

    Vector<SharedPtr<WalCmd>> cmds;

    // the next Iterator of the entry in the wal file
};

} // namespace infinity

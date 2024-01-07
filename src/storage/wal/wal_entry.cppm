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

#include <typeinfo>

export module wal:wal_entry;

import stl;
import parser;
import data_block;
import table_def;

import infinity_exception;
import index_def;

namespace infinity {

export enum class WalCommandType : i8 {
    INVALID = 0,
    // -----------------------------
    // Catalog
    // -----------------------------
    CREATE_DATABASE = 1,
    DROP_DATABASE = 2,
    CREATE_TABLE = 3,
    DROP_TABLE = 4,
    ALTER_INFO = 5,
    CREATE_INDEX = 6,
    DROP_INDEX = 7,

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

// WalCommandType -> String
export struct WalCmd {
    virtual ~WalCmd() = default;

    virtual WalCommandType GetType() = 0;

    virtual bool operator==(const WalCmd &other) const { return typeid(*this) == typeid(other); }
    bool operator!=(const WalCmd &other) const { return !(*this == other); }
    // Estimated serialized size in bytes
    [[nodiscard]] virtual i32 GetSizeInBytes() const = 0;
    // Write to a char buffer
    virtual void WriteAdv(char *&ptr) const = 0;
    // Read from a serialized version
    static SharedPtr<WalCmd> ReadAdv(char *&ptr, i32 max_bytes);

    static String WalCommandTypeToString(WalCommandType type);
};

export struct WalCmdCreateDatabase : public WalCmd {
    explicit WalCmdCreateDatabase(String db_name_) : db_name(std::move(db_name_)) {}

    WalCommandType GetType() override { return WalCommandType::CREATE_DATABASE; }
    bool operator==(const WalCmd &other) const override {
        auto other_cmd = dynamic_cast<const WalCmdCreateDatabase *>(&other);
        return other_cmd != nullptr && IsEqual(db_name, other_cmd->db_name);
    }
    [[nodiscard]] i32 GetSizeInBytes() const override;
    void WriteAdv(char *&buf) const override;

    String db_name;
};

export struct WalCmdDropDatabase : public WalCmd {
    explicit WalCmdDropDatabase(String db_name_) : db_name(std::move(db_name_)) {}

    WalCommandType GetType() override { return WalCommandType::DROP_DATABASE; }
    bool operator==(const WalCmd &other) const override {
        auto other_cmd = dynamic_cast<const WalCmdDropDatabase *>(&other);
        return other_cmd != nullptr && IsEqual(db_name, other_cmd->db_name);
    }
    [[nodiscard]] i32 GetSizeInBytes() const override;
    void WriteAdv(char *&buf) const override;

    String db_name;
};

export struct WalCmdCreateTable : public WalCmd {
    WalCmdCreateTable(String db_name_, const SharedPtr<TableDef> &table_def_) : db_name(std::move(db_name_)), table_def(table_def_) {}

    WalCommandType GetType() override { return WalCommandType::CREATE_TABLE; }
    bool operator==(const WalCmd &other) const override;
    [[nodiscard]] i32 GetSizeInBytes() const override;
    void WriteAdv(char *&buf) const override;

    String db_name;
    SharedPtr<TableDef> table_def;
};

export struct WalCmdCreateIndex : public WalCmd {
    WalCmdCreateIndex(String db_name, String table_name, SharedPtr<IndexDef> index_def)
        : db_name_(std::move(db_name)), table_name_(std::move(table_name)), index_def_(std::move(index_def)) {}

    WalCommandType GetType() override { return WalCommandType::CREATE_INDEX; }

    bool operator==(const WalCmd &other) const override;

    [[nodiscard]] i32 GetSizeInBytes() const override;

    void WriteAdv(char *&buf) const override;

    String db_name_{};
    String table_name_{};
    SharedPtr<IndexDef> index_def_{};
};

export struct WalCmdDropTable : public WalCmd {
    WalCmdDropTable(const String &db_name_, const String &table_name_) : db_name(db_name_), table_name(table_name_) {}

    WalCommandType GetType() override { return WalCommandType::DROP_TABLE; }
    bool operator==(const WalCmd &other) const override {
        auto other_cmd = dynamic_cast<const WalCmdDropTable *>(&other);
        return other_cmd != nullptr && IsEqual(db_name, other_cmd->db_name) && IsEqual(table_name, other_cmd->table_name);
    }
    [[nodiscard]] i32 GetSizeInBytes() const override;
    void WriteAdv(char *&buf) const override;

    String db_name;
    String table_name;
};

export struct WalCmdDropIndex : public WalCmd {
    WalCmdDropIndex(const String &db_name, const String &table_name, const String &index_name)
        : db_name_(db_name), table_name_(table_name), index_name_(index_name) {}

    virtual WalCommandType GetType() override { return WalCommandType::DROP_INDEX; }

    bool operator==(const WalCmd &other) const override;

    i32 GetSizeInBytes() const override;

    void WriteAdv(char *&buf) const override;

    const String db_name_{};
    const String table_name_{};
    const String index_name_{};
};

export struct WalCmdImport : public WalCmd {
    WalCmdImport(String db_name_, String table_name_, String segment_dir_, u32 segment_id_, u16 block_entries_size_, Vector<u16> &row_counts_)
        : db_name(std::move(db_name_)), table_name(std::move(table_name_)), segment_dir(std::move(segment_dir_)), segment_id(segment_id_),
          block_entries_size(block_entries_size_), row_counts_(row_counts_) {}

    WalCommandType GetType() override { return WalCommandType::IMPORT; }
    bool operator==(const WalCmd &other) const override;
    [[nodiscard]] i32 GetSizeInBytes() const override;
    void WriteAdv(char *&buf) const override;

    String db_name;
    String table_name;
    String segment_dir;
    u32 segment_id;
    u16 block_entries_size;
    // row_counts_[i] means the number of rows in the i-th block entry
    Vector<u16> row_counts_;
};

export struct WalCmdAppend : public WalCmd {
    WalCmdAppend(String db_name_, String table_name_, const SharedPtr<DataBlock> &block_)
        : db_name(std::move(db_name_)), table_name(std::move(table_name_)), block(block_) {}

    WalCommandType GetType() override { return WalCommandType::APPEND; }
    bool operator==(const WalCmd &other) const override;
    [[nodiscard]] i32 GetSizeInBytes() const override;
    void WriteAdv(char *&buf) const override;

    String db_name;
    String table_name;
    SharedPtr<DataBlock> block;
};

export struct WalCmdDelete : public WalCmd {
    WalCmdDelete(String db_name_, String table_name_, const Vector<RowID> &row_ids_)
        : db_name(std::move(db_name_)), table_name(std::move(table_name_)), row_ids(row_ids_) {}

    WalCommandType GetType() override { return WalCommandType::DELETE; }
    bool operator==(const WalCmd &other) const override;
    [[nodiscard]] i32 GetSizeInBytes() const override;
    void WriteAdv(char *&buf) const override;

    String db_name;
    String table_name;
    Vector<RowID> row_ids;
};

export struct WalCmdCheckpoint : public WalCmd {
    WalCmdCheckpoint(i64 max_commit_ts, bool is_full_checkpoint, String catalog_path)
        : max_commit_ts_(max_commit_ts), is_full_checkpoint_(is_full_checkpoint), catalog_path_(catalog_path) {}
    virtual WalCommandType GetType() override { return WalCommandType::CHECKPOINT; }

    virtual bool operator==(const WalCmd &other) const override;

    virtual i32 GetSizeInBytes() const override;

    void WriteAdv(char *&buf) const override;

    i64 max_commit_ts_;
    bool is_full_checkpoint_;
    String catalog_path_;
};

export struct WalEntryHeader {
    i32 size; // size of payload, including the header, round to multi
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
    [[nodiscard]] i32 GetSizeInBytes() const;

    // Write to a char buffer
    void WriteAdv(char *&ptr) const;
    // Read from a serialized version
    static SharedPtr<WalEntry> ReadAdv(char *&ptr, i32 max_bytes);

    Vector<SharedPtr<WalCmd>> cmds;

    [[nodiscard]] Pair<i64, String> GetCheckpointInfo() const;

    [[nodiscard]] bool IsCheckPoint() const;

    [[nodiscard]] bool IsFullCheckPoint() const;

    [[nodiscard]] String ToString() const;
};

export class WalEntryIterator {
public:
    static WalEntryIterator Make(const String &wal_path);

    [[nodiscard]] SharedPtr<WalEntry> Next();

private:
    WalEntryIterator(Vector<char> &&buf, StreamSize wal_size) : buf_(std::move(buf)), wal_size_(wal_size) {
        end_ = buf_.data() + wal_size_;
    }

    Vector<char> buf_{};
    StreamSize wal_size_{};
    char *end_{};
};

export class WalListIterator {
public:
    explicit WalListIterator(const Vector<String> &wal_list) {
        for (SizeT i = 0; i < wal_list.size(); ++i) {
            wal_deque_.push_back(wal_list[i]);
        }
    }

    [[nodiscard]] SharedPtr<WalEntry> Next();

private:
    Deque<String> wal_deque_{};
    UniquePtr<WalEntryIterator> iter_{};
};

} // namespace infinity

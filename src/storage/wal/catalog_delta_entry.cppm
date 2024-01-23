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

export module catalog_delta_entry;

import table_def;
import index_def;
import data_block;
import stl;
import parser;
import infinity_exception;
import catalog;
import third_party;

namespace infinity {

export enum class CatalogDeltaOperationType : i8 {
    INVALID = 0,
    // -----------------------------
    // Meta
    // -----------------------------
    ADD_DATABASE_META = 1,
    ADD_TABLE_META = 2,
    ADD_INDEX_META = 3,
    // -----------------------------
    // Entry
    // -----------------------------
    ADD_DATABASE_ENTRY = 11,
    ADD_TABLE_ENTRY = 12,
    ADD_SEGMENT_ENTRY = 13,
    ADD_BLOCK_ENTRY = 14,
    ADD_COLUMN_ENTRY = 15,

    // -----------------------------
    // INDEX
    // -----------------------------
    ADD_TABLE_INDEX_ENTRY = 21,
    ADD_IRS_INDEX_ENTRY = 22,
    ADD_COLUMN_INDEX_ENTRY = 23,
    ADD_SEGMENT_COLUMN_INDEX_ENTRY = 24,
};

/// class CatalogDeltaOperation
export class CatalogDeltaOperation {
public:
    CatalogDeltaOperation() = default;
    CatalogDeltaOperation(CatalogDeltaOperationType type) : type_(type) {}
    CatalogDeltaOperation(TxnTimeStamp begin_ts, bool is_delete) : begin_ts_(begin_ts), is_delete_(is_delete) {}
    virtual ~CatalogDeltaOperation() = default;
    virtual CatalogDeltaOperationType GetType() const = 0;
    [[nodiscard]] virtual SizeT GetSizeInBytes() const = 0;
    virtual void WriteAdv(char *&ptr) const = 0;
    static UniquePtr<CatalogDeltaOperation> ReadAdv(char *&ptr, i32 max_bytes);
    SizeT GetBaseSizeInBytes() const { return sizeof(TxnTimeStamp) + sizeof(bool); }
    virtual void SaveSate() = 0;
    virtual const String ToString() const = 0;
    virtual const String EncodeIndex() const = 0;

public:
    TxnTimeStamp begin_ts_{0};
    TransactionID txn_id_{0};
    TxnTimeStamp commit_ts_{0};
    bool is_delete_{false};

    bool is_flushed_{false};
    bool is_snapshotted_{false};
    CatalogDeltaOperationType type_{CatalogDeltaOperationType::INVALID};
};

/// class AddDBMetaOperation
export class AddDBMetaOperation : public CatalogDeltaOperation {
public:
    explicit AddDBMetaOperation(TxnTimeStamp begin_ts, bool is_delete, String db_name, String data_dir)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), data_dir_(std::move(data_dir)) {}
    explicit AddDBMetaOperation(DBMeta *db_meta) : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_DATABASE_META), db_meta_(db_meta) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_DATABASE_META; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size =
            sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->data_dir_.size() + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_DATABASE_META"; }
    const String EncodeIndex() const final { return String(fmt::format("{}#{}", i32(GetType()), this->db_name_)); }

public:
    DBMeta *db_meta_{};

private:
    String db_name_{};
    String data_dir_{};
};

/// class AddTableMetaOperation
export class AddTableMetaOperation : public CatalogDeltaOperation {
public:
    explicit AddTableMetaOperation(TxnTimeStamp begin_ts, bool is_delete, String table_name, String db_entry_dir_)
        : CatalogDeltaOperation(begin_ts, is_delete), table_name_(std::move(table_name)), db_entry_dir_(std::move(db_entry_dir_)) {}
    explicit AddTableMetaOperation(TableMeta *table_meta)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_TABLE_META), table_meta_(table_meta) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_TABLE_META; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->table_name_.size() + sizeof(i32) + this->db_entry_dir_.size() +
                          GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_TABLE_META"; }
    const String EncodeIndex() const final { return String(fmt::format("{}#{}#{}", i32(GetType()), db_name_, table_name_)); }

public:
    TableMeta *table_meta_{};

private:
    String db_name_{};
    String table_name_{};
    String db_entry_dir_{};
};

/// class AddDBEntryOperation
export class AddDBEntryOperation : public CatalogDeltaOperation {
public:
    explicit AddDBEntryOperation(TxnTimeStamp begin_ts, bool is_delete, String db_name, String db_entry_dir)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), db_entry_dir_(std::move(db_entry_dir)) {}
    explicit AddDBEntryOperation(SharedPtr<DBEntry> db_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_DATABASE_ENTRY), db_entry_(db_entry) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_DATABASE_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->db_entry_dir_.size() +
                          sizeof(bool) + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_DATABASE_ENTRY"; };
    const String EncodeIndex() const final { return String(fmt::format("{}#{}#{}", i32(GetType()), is_delete_, db_name_)); }

public:
    SharedPtr<DBEntry> db_entry_{};

private:
    String db_name_{};
    String db_entry_dir_{};
};

/// class AddTableEntryOperation
export class AddTableEntryOperation : public CatalogDeltaOperation {
public:
    explicit AddTableEntryOperation(TxnTimeStamp begin_ts, bool is_delete, String db_name, String table_name, String table_entry_dir)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          table_entry_dir_(std::move(table_entry_dir)) {}
    explicit AddTableEntryOperation(SharedPtr<TableEntry> table_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_TABLE_ENTRY), table_entry_(table_entry) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_TABLE_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          sizeof(i32) + this->table_entry_dir_.size() + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_TABLE_ENTRY"; }
    const String EncodeIndex() const final { return String(fmt::format("{}#{}#{}#{}", i32(GetType()), is_delete_, db_name_, table_name_)); }

public:
    SharedPtr<TableEntry> table_entry_{};

private:
    String db_name_{};
    String table_name_{};
    String table_entry_dir_{};
};

/// class AddSegmentEntryOperation
export class AddSegmentEntryOperation : public CatalogDeltaOperation {
public:
    explicit AddSegmentEntryOperation(TxnTimeStamp begin_ts,
                                      bool is_delete,
                                      String db_name,
                                      String table_name,
                                      SegmentID segment_id,
                                      String segment_dir)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)), segment_id_(segment_id),
          segment_dir_(std::move(segment_dir)) {}
    explicit AddSegmentEntryOperation(SegmentEntry *segment_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_SEGMENT_ENTRY), segment_entry_(segment_entry) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_SEGMENT_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          sizeof(i32) + this->segment_dir_.size() + sizeof(u32) + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_SEGMENT_ENTRY"; }
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}", i32(GetType()), this->db_name_, this->table_name_, this->segment_id_));
    }

public:
    SegmentEntry *segment_entry_{};

private:
    String db_name_{};
    String table_name_{};
    SegmentID segment_id_{};
    String segment_dir_{};
};

/// class AddBlockEntryOperation
export class AddBlockEntryOperation : public CatalogDeltaOperation {
public:
    // For create
    AddBlockEntryOperation(TxnTimeStamp begin_ts,
                           bool is_delete,
                           String db_name,
                           String table_name,
                           SegmentID segment_id,
                           BlockID block_id,
                           String block_dir)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)), segment_id_(segment_id),
          block_id_(block_id), block_dir_(std::move(block_dir)) {}
    // For update
    AddBlockEntryOperation(TxnTimeStamp begin_ts,
                           bool is_delete,
                           String db_name,
                           String table_name,
                           SegmentID segment_id,
                           BlockID block_id,
                           String block_dir,
                           u16 row_count,
                           u16 row_capacity)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)), segment_id_(segment_id),
          block_id_(block_id), block_dir_(std::move(block_dir)), row_count_(row_count), row_capacity_(row_capacity) {}
    explicit AddBlockEntryOperation(BlockEntry *block_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_BLOCK_ENTRY), block_entry_(block_entry) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_BLOCK_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          +sizeof(SegmentID) + sizeof(BlockID) + sizeof(i32) + this->block_dir_.size() + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_BLOCK_ENTRY"; }
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}", i32(GetType()), db_name_, table_name_, segment_id_, block_id_));
    }

public:
    BlockEntry *block_entry_{};

private:
    String db_name_{};
    String table_name_{};
    SegmentID segment_id_{0};
    BlockID block_id_{0};
    String block_dir_{};

private:
    // For update
    u16 row_count_{0};
    u16 row_capacity_{0};
};

/// class AddColumnEntryOperation
export class AddColumnEntryOperation : public CatalogDeltaOperation {
public:
    // For create
    explicit AddColumnEntryOperation(TxnTimeStamp begin_ts,
                                     bool is_delete,
                                     String db_name,
                                     String table_name,
                                     u32 segment_id,
                                     u16 block_id,
                                     u64 column_id)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)), segment_id_(segment_id),
          block_id_(block_id), column_id_(column_id) {}
    // For update
    explicit AddColumnEntryOperation(TxnTimeStamp begin_ts,
                                     bool is_delete,
                                     String db_name,
                                     String table_name,
                                     u32 segment_id,
                                     u16 block_id,
                                     u64 column_id,
                                     i32 next_line_idx)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)), segment_id_(segment_id),
          block_id_(block_id), column_id_(column_id), next_outline_idx_(next_line_idx) {}
    explicit AddColumnEntryOperation(BlockColumnEntry *column_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_COLUMN_ENTRY), column_entry_(column_entry) {}

    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_COLUMN_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          +sizeof(SegmentID) + sizeof(BlockID) + sizeof(ColumnID) + sizeof(i32) + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_COLUMN_ENTRY"; }
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}#{}", i32(GetType()), db_name_, table_name_, segment_id_, block_id_, column_id_));
    }

public:
    BlockColumnEntry *column_entry_{};

private:
    String db_name_{};
    String table_name_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    ColumnID column_id_{};
    i32 next_outline_idx_{-1}; // -1 for not having outline info
};

class TableIndexMeta;
/// class AddIndexMetaOperation
export class AddIndexMetaOperation : public CatalogDeltaOperation {
public:
    explicit AddIndexMetaOperation(TxnTimeStamp begin_ts, bool is_delete, String db_name, String table_name, String index_name)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          index_name_(std::move(index_name)) {}
    explicit AddIndexMetaOperation(TableIndexMeta *index_meta)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_INDEX_META), index_meta_(index_meta) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_INDEX_META; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          sizeof(i32) + this->index_name_.size() + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_INDEX_META"; }
    const String EncodeIndex() const final { return String(fmt::format("{}#{}#{}#{}", i32(GetType()), db_name_, table_name_, index_name_)); }

public:
    TableIndexMeta *index_meta_{};

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
};

/// class AddTableIndexEntryOperation
export class AddTableIndexEntryOperation : public CatalogDeltaOperation {
public:
    explicit AddTableIndexEntryOperation(TxnTimeStamp begin_ts, bool is_delete, String db_name, String table_name, String index_name)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          index_name_(std::move(index_name)) {}
    explicit AddTableIndexEntryOperation(SharedPtr<TableIndexEntry> table_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_TABLE_INDEX_ENTRY), table_index_entry_(table_index_entry) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_TABLE_INDEX_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          sizeof(i32) + this->index_name_.size() + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_TABLE_INDEX_ENTRY"; }
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}", i32(GetType()), is_delete_, db_name_, table_name_, index_name_));
    }

public:
    SharedPtr<TableIndexEntry> table_index_entry_{};

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    SharedPtr<IndexDef> index_def_{};
    String index_dir_{};
};

/// class AddIrsIndexEntryOperation
export class AddIrsIndexEntryOperation : public CatalogDeltaOperation {
public:
    explicit AddIrsIndexEntryOperation(TxnTimeStamp begin_ts, bool is_delete, String db_name, String table_name, String index_name, String index_dir)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          index_name_(std::move(index_name)), index_dir_(std::move(index_dir)) {}
    explicit AddIrsIndexEntryOperation(SharedPtr<IrsIndexEntry> irs_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_IRS_INDEX_ENTRY), irs_index_entry_(irs_index_entry) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_IRS_INDEX_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          sizeof(i32) + this->index_name_.size() + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_IRS_INDEX_ENTRY"; }
    const String EncodeIndex() const final { return String(fmt::format("{}#{}#{}#{}", i32(GetType()), db_name_, table_name_, index_name_)); }

public:
    SharedPtr<IrsIndexEntry> irs_index_entry_{};

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    String index_dir_{};
};

/// class AddColumnIndexEntryOperation
export class AddColumnIndexEntryOperation : public CatalogDeltaOperation {
public:
    explicit AddColumnIndexEntryOperation(TxnTimeStamp begin_ts,
                                          bool is_delete,
                                          String db_name,
                                          String table_name,
                                          String index_name,
                                          ColumnID column_id)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          index_name_(std::move(index_name)), column_id_(column_id) {}
    explicit AddColumnIndexEntryOperation(SharedPtr<ColumnIndexEntry> column_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_COLUMN_INDEX_ENTRY), column_index_entry_(column_index_entry) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_COLUMN_INDEX_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          sizeof(i32) + this->index_name_.size() + sizeof(ColumnID) + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_COLUMN_INDEX_ENTRY"; }
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}", i32(GetType()), db_name_, table_name_, index_name_, column_id_));
    }

public:
    SharedPtr<ColumnIndexEntry> column_index_entry_{};

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    String col_index_dir_{};
    ColumnID column_id_{};
};

/// class AddSegmentColumnEntryOperation
export class AddSegmentColumnIndexEntryOperation : public CatalogDeltaOperation {
public:
    explicit AddSegmentColumnIndexEntryOperation(TxnTimeStamp begin_ts,
                                                 bool is_delete,
                                                 String db_name,
                                                 String table_name,
                                                 String index_name,
                                                 u64 column_id,
                                                 u32 segment_id)
        : CatalogDeltaOperation(begin_ts, is_delete), db_name_(std::move(db_name)), table_name_(std::move(table_name)),
          index_name_(std::move(index_name)), column_id_(column_id), segment_id_(segment_id) {}
    explicit AddSegmentColumnIndexEntryOperation(SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOperationType::ADD_SEGMENT_COLUMN_INDEX_ENTRY), segment_column_index_entry_(segment_column_index_entry) {}
    CatalogDeltaOperationType GetType() const final { return CatalogDeltaOperationType::ADD_SEGMENT_COLUMN_INDEX_ENTRY; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOperationType) + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->table_name_.size() +
                          sizeof(i32) + this->index_name_.size() + sizeof(ColumnID) + sizeof(SegmentID) + GetBaseSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final { return "ADD_SEGMENT_COLUMN_INDEX_ENTRY"; }
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}#{}", i32(GetType()), db_name_, table_name_, index_name_, column_id_, segment_id_));
    }

public:
    SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry_{};

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    ColumnID column_id_{};
    SegmentID segment_id_{};
    TxnTimeStamp min_ts_{0};
    TxnTimeStamp max_ts_{0};
};

// size of payload, including the header, round to multi
// of 4. There's 4 bytes pad just after the payload storing
// the same value to assist backward iterating.
// payload. User shall populate it before writing to wal.
/// class CatalogDeltaEntryHeader
export class CatalogDeltaEntryHeader {
public:
    CatalogDeltaEntryHeader() = default;
    i32 size_{};
    u32 checksum_{};
    TransactionID txn_id_{};
    TxnTimeStamp commit_ts_{};
};

/// class CatalogDeltaEntry
export class CatalogDeltaEntry : CatalogDeltaEntryHeader {
public:
    CatalogDeltaEntry() = default;
    [[nodiscard]] i32 GetSizeInBytes() const;
    void WriteAdv(char *&ptr) const;
    static SharedPtr<CatalogDeltaEntry> ReadAdv(char *&ptr, i32 max_bytes);
    [[nodiscard]] String ToString() const;
    void SaveState(TransactionID txn_id, TxnTimeStamp commit_ts);
    TransactionID txn_id() const { return txn_id_; }
    void set_txn_id(TransactionID txn_id) { txn_id_ = txn_id; }
    TxnTimeStamp commit_ts() const { return commit_ts_; }
    void set_commit_ts(TransactionID commit_ts) { commit_ts_ = commit_ts; }
    Vector<UniquePtr<CatalogDeltaOperation>> &operations() { return operations_; }

private:
    Vector<UniquePtr<CatalogDeltaOperation>> operations_{};
};

export class GlobalCatalogDeltaEntry : public CatalogDeltaEntry {
public:
    GlobalCatalogDeltaEntry() = default;
    void Merge(UniquePtr<CatalogDeltaEntry> other);
    Deque<UniquePtr<CatalogDeltaOperation>> &global_operations() { return global_operations_; }
    HashMap<String, SizeT> &encode_op_to_id_map() { return encode_op_to_id_map_; }

private:
    Deque<UniquePtr<CatalogDeltaOperation>> global_operations_{};
    HashMap<String, SizeT> encode_op_to_id_map_{};
};

} // namespace infinity

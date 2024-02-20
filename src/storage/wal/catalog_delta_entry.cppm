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

import infinity_exception;
import db_meta;
import db_entry;
import table_entry;
import table_meta;
import segment_entry;
import block_entry;
import block_column_entry;
import table_index_entry;
import fulltext_index_entry;
import column_index_entry;
import segment_column_index_entry;
import catalog;
import serialize;
import third_party;
import table_entry_type;
import index_base;
import column_def;

namespace infinity {

export enum class CatalogDeltaOpType : i8 {
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
    ADD_FULLTEXT_INDEX_ENTRY = 22,
    ADD_COLUMN_INDEX_ENTRY = 23,
    ADD_SEGMENT_COLUMN_INDEX_ENTRY = 24,
};

/// class CatalogDeltaOperation
export class CatalogDeltaOperation {
public:
    CatalogDeltaOperation() = default;
    explicit CatalogDeltaOperation(CatalogDeltaOpType type) : type_(type) {}
    explicit CatalogDeltaOperation(CatalogDeltaOpType type, TxnTimeStamp begin_ts, bool is_delete, TransactionID txn_id, TxnTimeStamp commit_ts)
        : begin_ts_(begin_ts), txn_id_(txn_id), commit_ts_(commit_ts), is_delete_(is_delete), type_(type) {}
    virtual ~CatalogDeltaOperation() = default;
    virtual CatalogDeltaOpType GetType() const = 0;
    virtual String GetTypeStr() const = 0;
    [[nodiscard]] virtual SizeT GetSizeInBytes() const = 0;
    virtual void WriteAdv(char *&ptr) const = 0;
    static UniquePtr<CatalogDeltaOperation> ReadAdv(char *&ptr, i32 max_bytes);
    SizeT GetBaseSizeInBytes() const { return sizeof(TxnTimeStamp) + sizeof(bool) + sizeof(TransactionID) + sizeof(TxnTimeStamp); }
    void WriteAdvBase(char *&buf) const {
        WriteBufAdv(buf, this->type_);
        WriteBufAdv(buf, this->begin_ts_);
        WriteBufAdv(buf, this->is_delete_);
        WriteBufAdv(buf, this->txn_id_);
        WriteBufAdv(buf, this->commit_ts_);
    }
    static Tuple<TxnTimeStamp, bool, TransactionID, TxnTimeStamp> ReadAdvBase(char *&ptr) {
        TxnTimeStamp begin_ts = ReadBufAdv<TxnTimeStamp>(ptr);
        bool is_delete = ReadBufAdv<bool>(ptr);
        TransactionID txn_id = ReadBufAdv<TransactionID>(ptr);
        TxnTimeStamp commit_ts = ReadBufAdv<TxnTimeStamp>(ptr);
        return {begin_ts, is_delete, txn_id, commit_ts};
    }

    virtual void SaveSate() = 0;
    virtual const String ToString() const = 0;
    virtual const String EncodeIndex() const = 0;

    TxnTimeStamp begin_ts() { return begin_ts_; }
    TransactionID txn_id() { return txn_id_; }
    TxnTimeStamp commit_ts() { return commit_ts_; }
    bool is_delete() { return is_delete_; }

public:
    TxnTimeStamp begin_ts_{0};
    TransactionID txn_id_{0};
    TxnTimeStamp commit_ts_{0};
    bool is_delete_{false};

public:
    bool is_saved_sate_{false};
    CatalogDeltaOpType type_{CatalogDeltaOpType::INVALID};
};

/// class AddDBMetaOp
export class AddDBMetaOp : public CatalogDeltaOperation {
public:
    explicit AddDBMetaOp(TxnTimeStamp begin_ts, bool is_delete, TransactionID txn_id, TxnTimeStamp commit_ts, String db_name, String data_dir)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_META, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          data_dir_(std::move(data_dir)) {}
    explicit AddDBMetaOp(DBMeta *db_meta, TxnTimeStamp begin_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_META, begin_ts, false, 0, 0), db_meta_(db_meta) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_DATABASE_META; }
    String GetTypeStr() const final { return "ADD_DATABASE_META"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size =
            sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes() + sizeof(i32) + this->db_name_.size() + sizeof(i32) + this->data_dir_.size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("{}#{}#{}", i32(GetType()), txn_id_, this->db_name_)); }

public:
    const String &db_name() const { return db_name_; }
    const String &data_dir() const { return data_dir_; }

public:
    DBMeta *db_meta_{};

private:
    String db_name_{};
    String data_dir_{};
};

/// class AddTableMetaOp
export class AddTableMetaOp : public CatalogDeltaOperation {
public:
    explicit AddTableMetaOp(TxnTimeStamp begin_ts,
                            bool is_delete,
                            TransactionID txn_id,
                            TxnTimeStamp commit_ts,
                            String db_name,
                            String table_name,
                            String db_entry_dir)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_META, begin_ts, is_delete, txn_id, commit_ts), db_name_(db_name),
          table_name_(std::move(table_name)), db_entry_dir_(std::move(db_entry_dir)) {}
    explicit AddTableMetaOp(TableMeta *table_meta, TxnTimeStamp begin_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_META, begin_ts, false, 0, 0), table_meta_(table_meta) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_TABLE_META; }
    String GetTypeStr() const final { return "ADD_TABLE_META"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes() + sizeof(i32) + this->db_name_.size() + sizeof(i32) +
                          this->table_name_.size() + sizeof(i32) + this->db_entry_dir_.size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("{}#{}#{}#{}", i32(GetType()), txn_id_, db_name_, table_name_)); }

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    const String &db_entry_dir() const { return db_entry_dir_; }

public:
    TableMeta *table_meta_{};

private:
    String db_name_{};
    String table_name_{};
    String db_entry_dir_{};
};

/// class AddDBEntryOp
export class AddDBEntryOp : public CatalogDeltaOperation {
public:
    explicit AddDBEntryOp(TxnTimeStamp begin_ts, bool is_delete, TransactionID txn_id, TxnTimeStamp commit_ts, String db_name, String db_entry_dir)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          db_entry_dir_(std::move(db_entry_dir)) {}
    explicit AddDBEntryOp(SharedPtr<DBEntry> db_entry) : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_ENTRY), db_entry_(db_entry) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_DATABASE_ENTRY; }
    String GetTypeStr() const final { return "ADD_DATABASE_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += +sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->db_entry_dir_.size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("{}#{}#{}#{}", i32(GetType()), txn_id_, is_delete_, db_name_)); }

public:
    const String &db_name() const { return db_name_; }
    const String &data_dir() const { return db_entry_dir_; }

public:
    SharedPtr<DBEntry> db_entry_{};

private:
    String db_name_{};
    String db_entry_dir_{};
};

/// class AddTableEntryOp
export class AddTableEntryOp : public CatalogDeltaOperation {
public:
    explicit AddTableEntryOp(TxnTimeStamp begin_ts,
                             bool is_delete,
                             TransactionID txn_id,
                             TxnTimeStamp commit_ts,
                             String db_name,
                             String table_name,
                             String table_entry_dir,
                             Vector<SharedPtr<ColumnDef>> column_defs)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), table_entry_dir_(std::move(table_entry_dir)), column_defs_(column_defs) {}

    explicit AddTableEntryOp(TxnTimeStamp begin_ts,
                             bool is_delete,
                             TransactionID txn_id,
                             TxnTimeStamp commit_ts,
                             String db_name,
                             String table_name,
                             String table_entry_dir,
                             Vector<SharedPtr<ColumnDef>> column_defs,
                             SizeT row_count)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), table_entry_dir_(std::move(table_entry_dir)), column_defs_(column_defs), row_count_(row_count) {}

    explicit AddTableEntryOp(SharedPtr<TableEntry> table_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY), table_entry_(table_entry) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_TABLE_ENTRY; }
    String GetTypeStr() const final { return "ADD_TABLE_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = 0;
        total_size += sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(i32) + this->table_entry_dir_.size();

        total_size += sizeof(i32);
        for (u32 i = 0; i < column_defs_.size(); i++) {
            const ColumnDef &cd = *column_defs_[i];
            total_size += sizeof(i64);
            total_size += cd.column_type_->GetSizeInBytes();
            total_size += sizeof(i32) + cd.name_.length();
            total_size += sizeof(i32);
            total_size += cd.constraints_.size() * sizeof(ConstraintType);
        }

        total_size += sizeof(SizeT);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}", i32(GetType()), txn_id_, is_delete_, db_name_, table_name_));
    }

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    const String &table_entry_dir() const { return table_entry_dir_; }
    const Vector<SharedPtr<ColumnDef>> &column_defs() const { return column_defs_; }
    TableEntryType table_entry_type() const { return table_entry_type_; }
    SizeT row_count() const { return row_count_; }

public:
    SharedPtr<TableEntry> table_entry_{};

private:
    String db_name_{};
    String table_name_{};
    String table_entry_dir_{};
    Vector<SharedPtr<ColumnDef>> column_defs_{};
    TableEntryType table_entry_type_{TableEntryType::kTableEntry};

private:
    SizeT row_count_{0};
};

/// class AddSegmentEntryOp
export class AddSegmentEntryOp : public CatalogDeltaOperation {
public:
    explicit AddSegmentEntryOp(TxnTimeStamp begin_ts,
                               bool is_delete,
                               TransactionID txn_id,
                               TxnTimeStamp commit_ts,
                               String db_name,
                               String table_name,
                               SegmentID segment_id,
                               String segment_dir,
                               u64 column_count,
                               SizeT row_count,
                               SizeT row_capacity,
                               SizeT actual_row_count,
                               TxnTimeStamp min_row_ts,
                               TxnTimeStamp max_row_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), segment_id_(segment_id), segment_dir_(std::move(segment_dir)), column_count_(column_count),
          row_count_(row_count), actual_row_count_(actual_row_count), row_capacity_(row_capacity), min_row_ts_(min_row_ts), max_row_ts_(max_row_ts) {}
    explicit AddSegmentEntryOp(SegmentEntry *segment_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_ENTRY), segment_entry_(segment_entry) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_SEGMENT_ENTRY; }
    String GetTypeStr() const final { return "ADD_SEGMENT_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = 0;
        total_size += sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(SegmentID);
        total_size += sizeof(i32) + this->segment_dir_.size();
        total_size += sizeof(u64);
        total_size += sizeof(SizeT);
        total_size += sizeof(actual_row_count_);
        total_size += sizeof(SizeT);
        total_size += sizeof(TxnTimeStamp) * 2;
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}", i32(GetType()), txn_id_, this->db_name_, this->table_name_, this->segment_id_));
    }
    void FlushDataToDisk(TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    const String &segment_dir() const { return segment_dir_; }
    SegmentID segment_id() const { return segment_id_; }
    u64 column_count() const { return column_count_; }
    SizeT row_count() const { return row_count_; }
    SizeT actual_row_count() const { return actual_row_count_; }
    SizeT row_capacity() const { return row_capacity_; }
    TxnTimeStamp min_row_ts() const { return min_row_ts_; }
    TxnTimeStamp max_row_ts() const { return max_row_ts_; }

public:
    SegmentEntry *segment_entry_{};

private:
    String db_name_{};
    String table_name_{};
    SegmentID segment_id_{};
    String segment_dir_{};

private:
    u64 column_count_{0};
    SizeT row_count_{0};
    SizeT actual_row_count_{0};
    SizeT row_capacity_{0};
    TxnTimeStamp min_row_ts_{0};
    TxnTimeStamp max_row_ts_{0};
};

/// class AddBlockEntryOp
export class AddBlockEntryOp : public CatalogDeltaOperation {
public:
    // For create
    AddBlockEntryOp(TxnTimeStamp begin_ts,
                    bool is_delete,
                    TransactionID txn_id,
                    TxnTimeStamp commit_ts,
                    String db_name,
                    String table_name,
                    SegmentID segment_id,
                    BlockID block_id,
                    String block_dir)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_BLOCK_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), segment_id_(segment_id), block_id_(block_id), block_dir_(std::move(block_dir)) {}
    // For update
    explicit AddBlockEntryOp(TxnTimeStamp begin_ts,
                             bool is_delete,
                             TransactionID txn_id,
                             TxnTimeStamp commit_ts,
                             String db_name,
                             String table_name,
                             SegmentID segment_id,
                             BlockID block_id,
                             String block_dir,
                             u16 row_count,
                             u16 row_capacity,
                             TxnTimeStamp min_row_ts,
                             TxnTimeStamp max_row_ts,
                             TxnTimeStamp checkpoint_ts,
                             u16 checkpoint_row_count)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_BLOCK_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), segment_id_(segment_id), block_id_(block_id), block_dir_(std::move(block_dir)),
          row_capacity_(row_capacity), row_count_(row_count), min_row_ts_(min_row_ts), max_row_ts_(max_row_ts), checkpoint_ts_(checkpoint_ts),
          checkpoint_row_count_(checkpoint_row_count) {}
    explicit AddBlockEntryOp(BlockEntry *block_entry) : CatalogDeltaOperation(CatalogDeltaOpType::ADD_BLOCK_ENTRY), block_entry_(block_entry) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_BLOCK_ENTRY; }
    String GetTypeStr() const final { return "ADD_BLOCK_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(SegmentID) + sizeof(BlockID);
        total_size += sizeof(i32) + this->block_dir_.size();
        total_size += sizeof(u16) + sizeof(u16) + sizeof(TxnTimeStamp) * 2;
        total_size += sizeof(TxnTimeStamp) + sizeof(u16);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}#{}", i32(GetType()), txn_id_, db_name_, table_name_, segment_id_, block_id_));
    }

public:
    BlockEntry *block_entry_{};

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    SegmentID segment_id() const { return segment_id_; }
    BlockID block_id() const { return block_id_; }
    const String &block_dir() const { return block_dir_; }
    u16 row_count() const { return row_count_; }
    u16 row_capacity() const { return row_capacity_; }
    TxnTimeStamp min_row_ts() const { return min_row_ts_; }
    TxnTimeStamp max_row_ts() const { return max_row_ts_; }
    TxnTimeStamp checkpoint_ts() const { return checkpoint_ts_; }
    u16 checkpoint_row_count() const { return checkpoint_row_count_; }

private:
    String db_name_{};
    String table_name_{};
    SegmentID segment_id_{0};
    BlockID block_id_{0};
    String block_dir_{};

private:
    // For update
    u16 row_capacity_{0};
    u16 row_count_{0};
    TxnTimeStamp min_row_ts_{0};
    TxnTimeStamp max_row_ts_{0};
    TxnTimeStamp checkpoint_ts_{0};
    u16 checkpoint_row_count_{0};
};

/// class AddColumnEntryOp
export class AddColumnEntryOp : public CatalogDeltaOperation {
public:
    // For create
    explicit AddColumnEntryOp(TxnTimeStamp begin_ts,
                              bool is_delete,
                              TransactionID txn_id,
                              TxnTimeStamp commit_ts,
                              String db_name,
                              String table_name,
                              u32 segment_id,
                              u16 block_id,
                              u64 column_id)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), segment_id_(segment_id), block_id_(block_id), column_id_(column_id) {}
    // For update
    explicit AddColumnEntryOp(TxnTimeStamp begin_ts,
                              bool is_delete,
                              TransactionID txn_id,
                              TxnTimeStamp commit_ts,
                              String db_name,
                              String table_name,
                              SegmentID segment_id,
                              BlockID block_id,
                              ColumnID column_id,
                              i32 next_line_idx)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), segment_id_(segment_id), block_id_(block_id), column_id_(column_id), next_outline_idx_(next_line_idx) {}
    explicit AddColumnEntryOp(BlockColumnEntry *column_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY), column_entry_(column_entry) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_COLUMN_ENTRY; }
    String GetTypeStr() const final { return "ADD_COLUMN_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(SegmentID) + sizeof(BlockID) + sizeof(ColumnID) + sizeof(i32);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}#{}#{}", i32(GetType()), txn_id_, db_name_, table_name_, segment_id_, block_id_, column_id_));
    }

public:
    BlockColumnEntry *column_entry_{};

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    SegmentID segment_id() const { return segment_id_; }
    BlockID block_id() const { return block_id_; }
    ColumnID column_id() const { return column_id_; }
    i32 next_outline_idx() const { return next_outline_idx_; }

private:
    String db_name_{};
    String table_name_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    ColumnID column_id_{};
    i32 next_outline_idx_{0};
};

class TableIndexMeta;
/// class AddIndexMetaOp
export class AddIndexMetaOp : public CatalogDeltaOperation {
public:
    explicit AddIndexMetaOp(TxnTimeStamp begin_ts,
                            bool is_delete,
                            TransactionID txn_id,
                            TxnTimeStamp commit_ts,
                            String db_name,
                            String table_name,
                            String index_name)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_INDEX_META, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)) {}
    explicit AddIndexMetaOp(TableIndexMeta *index_meta, TxnTimeStamp begin_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_INDEX_META, begin_ts, false, 0, 0), index_meta_(index_meta) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_INDEX_META; }
    String GetTypeStr() const final { return "ADD_INDEX_META"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(i32) + this->index_name_.size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}", i32(GetType()), txn_id_, db_name_, table_name_, index_name_));
    }

public:
    TableIndexMeta *index_meta_{};

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    const String &index_name() const { return index_name_; }

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
};

/// class AddTableIndexEntryOp
export class AddTableIndexEntryOp : public CatalogDeltaOperation {
public:
    explicit AddTableIndexEntryOp(TxnTimeStamp begin_ts,
                                  bool is_delete,
                                  TransactionID txn_id,
                                  TxnTimeStamp commit_ts,
                                  String db_name,
                                  String table_name,
                                  String index_name,
                                  String index_dir,
                                  SharedPtr<IndexDef> index_def)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)), index_dir_(std::move(index_dir)), index_def_(std::move(index_def)) {
    }
    explicit AddTableIndexEntryOp(SharedPtr<TableIndexEntry> table_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY), table_index_entry_(table_index_entry) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_TABLE_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(i32) + this->index_name_.size();
        total_size += sizeof(i32) + this->index_dir_.size();
        total_size += this->index_def_->GetSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}#{}", i32(GetType()), txn_id_, is_delete_, db_name_, table_name_, index_name_));
    }

public:
    SharedPtr<TableIndexEntry> table_index_entry_{};

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    const String &index_name() const { return index_name_; }
    const String &index_dir() const { return index_dir_; }
    SharedPtr<IndexDef> index_def() const { return index_def_; }

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    String index_dir_{};
    SharedPtr<IndexDef> index_def_{};
};

/// class AddFulltextIndexEntryOp
export class AddFulltextIndexEntryOp : public CatalogDeltaOperation {
public:
    explicit AddFulltextIndexEntryOp(TxnTimeStamp begin_ts,
                                     bool is_delete,
                                     TransactionID txn_id,
                                     TxnTimeStamp commit_ts,
                                     String db_name,
                                     String table_name,
                                     String index_name,
                                     String index_dir)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)), index_dir_(std::move(index_dir)) {}
    explicit AddFulltextIndexEntryOp(SharedPtr<FulltextIndexEntry> fulltext_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY), fulltext_index_entry_(fulltext_index_entry) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_FULLTEXT_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(i32) + this->index_name_.size();
        total_size += sizeof(i32) + this->index_dir_.size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}", i32(GetType()), txn_id_, db_name_, table_name_, index_name_));
    }

public:
    SharedPtr<FulltextIndexEntry> fulltext_index_entry_{};

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    const String &index_name() const { return index_name_; }
    const String &index_dir() const { return index_dir_; }

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    String index_dir_{};
};

/// class AddColumnIndexEntryOp
export class AddColumnIndexEntryOp : public CatalogDeltaOperation {
public:
    explicit AddColumnIndexEntryOp(TxnTimeStamp begin_ts,
                                   bool is_delete,
                                   TransactionID txn_id,
                                   TxnTimeStamp commit_ts,
                                   String db_name,
                                   String table_name,
                                   String index_name,
                                   String col_index_dir,
                                   ColumnID column_id,
                                   SharedPtr<IndexBase> index_base)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_INDEX_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)), col_index_dir_(std::move(col_index_dir)), column_id_(column_id),
          index_base_(index_base) {}
    explicit AddColumnIndexEntryOp(SharedPtr<ColumnIndexEntry> column_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_INDEX_ENTRY), column_index_entry_(column_index_entry) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_COLUMN_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_COLUMN_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        SizeT total_size = 0;
        total_size += sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(i32) + this->index_name_.size();
        total_size += sizeof(i32) + this->col_index_dir_.size();
        total_size += sizeof(ColumnID);
        total_size += index_base_->GetSizeInBytes();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}#{}", i32(GetType()), txn_id_, db_name_, table_name_, index_name_, column_id_));
    }

public:
    SharedPtr<ColumnIndexEntry> column_index_entry_{};

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    const String &index_name() const { return index_name_; }
    const String &col_index_dir() const { return col_index_dir_; }
    ColumnID column_id() const { return column_id_; }
    SharedPtr<IndexBase> index_base() const { return index_base_; }

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    String col_index_dir_{};
    ColumnID column_id_{};
    SharedPtr<IndexBase> index_base_{};
};

/// class AddSegmentColumnEntryOperation
export class AddSegmentColumnIndexEntryOp : public CatalogDeltaOperation {
public:
    explicit AddSegmentColumnIndexEntryOp(TxnTimeStamp begin_ts,
                                          bool is_delete,
                                          TransactionID txn_id,
                                          TxnTimeStamp commit_ts,
                                          String db_name,
                                          String table_name,
                                          String index_name,
                                          ColumnID column_id,
                                          SegmentID segment_id,
                                          TxnTimeStamp min_ts,
                                          TxnTimeStamp max_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_COLUMN_INDEX_ENTRY, begin_ts, is_delete, txn_id, commit_ts),
          db_name_(std::move(db_name)), table_name_(std::move(table_name)), index_name_(std::move(index_name)), column_id_(column_id),
          segment_id_(segment_id), min_ts_(min_ts), max_ts_(max_ts) {}
    explicit AddSegmentColumnIndexEntryOp(SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_COLUMN_INDEX_ENTRY), segment_column_index_entry_(segment_column_index_entry) {}
    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_SEGMENT_COLUMN_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_SEGMENT_COLUMN_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_.size();
        total_size += sizeof(i32) + this->table_name_.size();
        total_size += sizeof(i32) + this->index_name_.size();
        total_size += sizeof(ColumnID) + sizeof(SegmentID) + sizeof(TxnTimeStamp) + sizeof(TxnTimeStamp);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    void SaveSate() final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("{}#{}#{}#{}#{}#{}#{}", i32(GetType()), txn_id_, db_name_, table_name_, index_name_, column_id_, segment_id_));
    }
    void Flush(TxnTimeStamp max_commit_ts);

public:
    SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry_{};

public:
    const String &db_name() const { return db_name_; }
    const String &table_name() const { return table_name_; }
    const String &index_name() const { return index_name_; }
    ColumnID column_id() const { return column_id_; }
    SegmentID segment_id() const { return segment_id_; }
    TxnTimeStamp min_ts() const { return min_ts_; }
    TxnTimeStamp max_ts() const { return max_ts_; }

private:
    String db_name_{};
    String table_name_{};
    String index_name_{};
    ColumnID column_id_{0};
    SegmentID segment_id_{0};
    TxnTimeStamp min_ts_{0};
    TxnTimeStamp max_ts_{0};
};

// size of payload, including the header, round to multi
// of 4. There's 4 bytes pad just after the payload storing
// the same value to assist backward iterating.
// payload. User shall populate it before writing to wal.
/// class CatalogDeltaEntryHeader
export struct CatalogDeltaEntryHeader {
    i32 size_{}; // size of payload, including the header, round to multi
    // of 4. There's 4 bytes pad just after the payload storing
    // the same value to assist backward iterating.
    u32 checksum_{}; // crc32 of the entry, including the header and the
    // payload. User shall populate it before writing to wal.
    TransactionID txn_id_{};   // txn id of the entry
    TxnTimeStamp commit_ts_{}; // commit timestamp of the txn
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

    // called by `AddCatalogDeltaOperation`
    void AddOperation(UniquePtr<CatalogDeltaOperation> operation) {
        std::lock_guard lock(mtx_);
        operations_.emplace_back(std::move(operation));
    }

    // Pick and remove all operations that are committed before `max_commit_ts`
    UniquePtr<CatalogDeltaEntry> PickFlushEntry(TxnTimeStamp max_commit_ts);
    // FIXME:  should not contain mutex so that it can be moved out

public:
    // Attention: only use in unit test or thread safe context
    Vector<UniquePtr<CatalogDeltaOperation>> &operations() { return operations_; }

protected:
    std::mutex mtx_{};
    Vector<UniquePtr<CatalogDeltaOperation>> operations_{};
};

export class GlobalCatalogDeltaEntry : public CatalogDeltaEntry {
public:
    GlobalCatalogDeltaEntry() = default;
    [[nodiscard]] String ToString() const;
    void Merge(UniquePtr<CatalogDeltaEntry> other);
    HashMap<String, SizeT> &encode_to_id_map() { return encode_to_id_map_; }

private:
    HashMap<String, SizeT> encode_to_id_map_{};
};

} // namespace infinity

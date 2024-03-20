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
import data_block;
import stl;

import infinity_exception;
import db_entry;
import table_entry;
import segment_entry;
import block_entry;
import block_column_entry;
import table_index_entry;
import fulltext_index_entry;
import segment_index_entry;
import catalog;
import serialize;
import third_party;
import table_entry_type;
import index_base;
import column_def;
import base_entry;
import default_values;

namespace infinity {

export enum class CatalogDeltaOpType : i8 {
    INVALID = 0,
    // -----------------------------
    // Entry
    // -----------------------------
    ADD_DATABASE_ENTRY = 1,
    ADD_TABLE_ENTRY = 2,
    ADD_SEGMENT_ENTRY = 3,
    ADD_BLOCK_ENTRY = 4,
    ADD_COLUMN_ENTRY = 5,

    // -----------------------------
    // INDEX
    // -----------------------------
    ADD_TABLE_INDEX_ENTRY = 11,
    ADD_FULLTEXT_INDEX_ENTRY = 12,
    ADD_SEGMENT_INDEX_ENTRY = 13,

    // -----------------------------
    // SEGMENT STATUS
    // -----------------------------
    SET_SEGMENT_STATUS_SEALED = 21,
};

/// class CatalogDeltaOperation
export class CatalogDeltaOperation {
public:
    CatalogDeltaOperation() = default;                                                                              // TODO: remove it
    explicit CatalogDeltaOperation(CatalogDeltaOpType type) : type_(type) {}                                        // TODO: remove it
    explicit CatalogDeltaOperation(CatalogDeltaOpType type, bool is_delete) : is_delete_(is_delete), type_(type) {} // TODO: remove it
    explicit CatalogDeltaOperation(CatalogDeltaOpType type, TxnTimeStamp begin_ts, bool is_delete, TransactionID txn_id, TxnTimeStamp commit_ts)
        : begin_ts_(begin_ts), txn_id_(txn_id), commit_ts_(commit_ts), is_delete_(is_delete), type_(type) {}
    explicit CatalogDeltaOperation(CatalogDeltaOpType type, BaseEntry *base_entry)
        : begin_ts_(base_entry->begin_ts_), txn_id_(base_entry->txn_id_), commit_ts_(base_entry->commit_ts_), is_delete_(base_entry->deleted_),
          type_(type) {}
    virtual ~CatalogDeltaOperation(){};
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

    virtual const String ToString() const = 0;
    virtual const String EncodeIndex() const = 0;
    virtual bool operator==(const CatalogDeltaOperation &rhs) const;
    virtual void Merge(UniquePtr<CatalogDeltaOperation> other) = 0;

    TxnTimeStamp begin_ts() { return begin_ts_; }
    TransactionID txn_id() { return txn_id_; }
    TxnTimeStamp commit_ts() { return commit_ts_; }
    bool is_delete() const { return is_delete_; }

public:
    TxnTimeStamp begin_ts_{0};
    TransactionID txn_id_{0};
    TxnTimeStamp commit_ts_{0};
    bool is_delete_{false};

public:
    bool is_saved_sate_{false};
    CatalogDeltaOpType type_{CatalogDeltaOpType::INVALID};
};

/// class AddDBEntryOp
export class AddDBEntryOp : public CatalogDeltaOperation {
public:
    explicit AddDBEntryOp(TxnTimeStamp begin_ts,
                          bool is_delete,
                          TransactionID txn_id,
                          TxnTimeStamp commit_ts,
                          SharedPtr<String> db_name,
                          SharedPtr<String> db_entry_dir)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          db_entry_dir_(std::move(db_entry_dir)) {}

    explicit AddDBEntryOp(DBEntry *db_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_ENTRY, db_entry), db_name_(db_entry->db_name_ptr()),
          db_entry_dir_(db_entry->db_entry_dir()) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_DATABASE_ENTRY; }
    String GetTypeStr() const final { return "ADD_DATABASE_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += +sizeof(i32) + db_name_->size();
        total_size += sizeof(i32) + db_entry_dir_->size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}", *db_name_)); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other) override;

public:
    const SharedPtr<String> &db_name() const { return db_name_; }
    const SharedPtr<String> &db_entry_dir() const { return db_entry_dir_; }

private:
    SharedPtr<String> db_name_{};
    SharedPtr<String> db_entry_dir_{};
};

/// class AddTableEntryOp
export class AddTableEntryOp : public CatalogDeltaOperation {
public:
    explicit AddTableEntryOp(TxnTimeStamp begin_ts,
                             bool is_delete,
                             TransactionID txn_id,
                             TxnTimeStamp commit_ts,
                             SharedPtr<String> db_name,
                             SharedPtr<String> table_name,
                             SharedPtr<String> table_entry_dir,
                             Vector<SharedPtr<ColumnDef>> column_defs,
                             SizeT row_count,
                             SegmentID unsealed_id)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), table_entry_dir_(std::move(table_entry_dir)), column_defs_(std::move(column_defs)),
          row_count_(row_count), unsealed_id_(unsealed_id) {}

    explicit AddTableEntryOp(TableEntry *table_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY, table_entry), db_name_(table_entry->GetDBName()),
          table_name_(table_entry->GetTableName()), table_entry_dir_(table_entry->TableEntryDir()), column_defs_(table_entry->column_defs()),
          row_count_(table_entry->row_count()), // TODO: fix it
          unsealed_id_(table_entry->unsealed_id()) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_TABLE_ENTRY; }
    String GetTypeStr() const final { return "ADD_TABLE_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = 0;
        total_size += sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(i32) + this->table_entry_dir_->size();

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
        total_size += sizeof(SegmentID);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}#{}", *db_name_, *table_name_)); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other) override;

public:
    const SharedPtr<String> &db_name() const { return db_name_; }
    const SharedPtr<String> &table_name() const { return table_name_; }
    const SharedPtr<String> &table_entry_dir() const { return table_entry_dir_; }
    const Vector<SharedPtr<ColumnDef>> &column_defs() const { return column_defs_; }
    TableEntryType table_entry_type() const { return table_entry_type_; }
    SizeT row_count() const { return row_count_; }
    SegmentID unsealed_id() const { return unsealed_id_; }

private:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> table_entry_dir_{};
    Vector<SharedPtr<ColumnDef>> column_defs_{};
    TableEntryType table_entry_type_{TableEntryType::kTableEntry};
    SizeT row_count_{0};
    SegmentID unsealed_id_{};
};

/// class AddSegmentEntryOp
export class AddSegmentEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddSegmentEntryOp> ReadAdv(char *&ptr);

    AddSegmentEntryOp() = default;

    explicit AddSegmentEntryOp(SegmentEntry *segment_entry, bool set_sealed = false)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_ENTRY, segment_entry), db_name_(segment_entry->GetTableEntry()->GetDBName()),
          table_name_(segment_entry->GetTableEntry()->GetTableName()), segment_id_(segment_entry->segment_id()), status_(segment_entry->status()),
          column_count_(segment_entry->column_count()), row_count_(segment_entry->row_count()), // FIXME: use append_state
          actual_row_count_(segment_entry->actual_row_count()),                                 // FIXME: use append_state
          row_capacity_(segment_entry->row_capacity()), min_row_ts_(segment_entry->min_row_ts()), max_row_ts_(segment_entry->max_row_ts()),
          deprecate_ts_(segment_entry->deprecate_ts()), set_sealed_(set_sealed) {
        if (set_sealed) {
            String segment_filter_binary_data = segment_entry->GetFastRoughFilter()->SerializeToString();
            this->segment_filter_binary_data_ = std::move(segment_filter_binary_data);
        }
    }

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_SEGMENT_ENTRY; }
    String GetTypeStr() const final { return "ADD_SEGMENT_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = 0;
        total_size += sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(SegmentID);
        total_size += sizeof(SegmentStatus);
        total_size += sizeof(u64);
        total_size += sizeof(SizeT);
        total_size += sizeof(actual_row_count_);
        total_size += sizeof(SizeT);
        total_size += sizeof(TxnTimeStamp) * 3;
        total_size += sizeof(bool);
        if (set_sealed_) {
            total_size += sizeof(i32) + this->segment_filter_binary_data_.size();
        }
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}#{}#{}", *this->db_name_, *this->table_name_, this->segment_id_)); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other) override;

public:
    const SharedPtr<String> &db_name() const { return db_name_; }
    const SharedPtr<String> &table_name() const { return table_name_; }
    SegmentStatus status() const { return status_; }
    SegmentID segment_id() const { return segment_id_; }
    u64 column_count() const { return column_count_; }
    SizeT row_count() const { return row_count_; }
    SizeT actual_row_count() const { return actual_row_count_; }
    SizeT row_capacity() const { return row_capacity_; }
    TxnTimeStamp min_row_ts() const { return min_row_ts_; }
    TxnTimeStamp max_row_ts() const { return max_row_ts_; }
    TxnTimeStamp deprecate_ts() const { return deprecate_ts_; }
    bool set_sealed() const { return set_sealed_; }
    const String &segment_filter_binary_data() const { return segment_filter_binary_data_; }

private:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SegmentID segment_id_{};

private:
    SegmentStatus status_{};
    u64 column_count_{0};
    SizeT row_count_{0};
    SizeT actual_row_count_{0};
    SizeT row_capacity_{0};
    TxnTimeStamp min_row_ts_{0};
    TxnTimeStamp max_row_ts_{0};
    TxnTimeStamp deprecate_ts_{0};

    // merge if set_sealed is true
    // following data include: 1. minmax filter for all valid columns 2. bloom filter for selected columns
    bool set_sealed_{false};
    String segment_filter_binary_data_{};
};

/// class AddBlockEntryOp
export class AddBlockEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddBlockEntryOp> ReadAdv(char *&ptr);

    AddBlockEntryOp() = default;

    explicit AddBlockEntryOp(BlockEntry *block_entry, bool set_sealed = false)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_BLOCK_ENTRY, block_entry), block_entry_(block_entry),
          db_name_(block_entry->GetSegmentEntry()->GetTableEntry()->GetDBName()),
          table_name_(block_entry->GetSegmentEntry()->GetTableEntry()->GetTableName()), segment_id_(block_entry->GetSegmentEntry()->segment_id()),
          block_id_(block_entry->block_id()), row_capacity_(block_entry->row_capacity()), row_count_(block_entry->row_count()),
          min_row_ts_(block_entry->min_row_ts()), max_row_ts_(block_entry->max_row_ts()), checkpoint_ts_(block_entry->checkpoint_ts()),
          checkpoint_row_count_(block_entry->checkpoint_row_count()), set_sealed_(set_sealed) {
        if (set_sealed) {
            String block_filter_binary_data = block_entry->GetFastRoughFilter()->SerializeToString();
            this->block_filter_binary_data_ = std::move(block_filter_binary_data);
        }
    }

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_BLOCK_ENTRY; }
    String GetTypeStr() const final { return "ADD_BLOCK_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(SegmentID) + sizeof(BlockID);
        total_size += sizeof(u16) + sizeof(u16) + sizeof(TxnTimeStamp) * 2;
        total_size += sizeof(TxnTimeStamp) + sizeof(u16);
        total_size += sizeof(bool);
        if (set_sealed_) {
            total_size += sizeof(i32) + this->block_filter_binary_data_.size();
        }
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}#{}#{}#{}", *db_name_, *table_name_, segment_id_, block_id_)); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other) override;

    void FlushDataToDisk(TxnTimeStamp max_commit_ts);

public:
    BlockEntry *block_entry_{};

public:
    const SharedPtr<String> &db_name() const { return db_name_; }
    const SharedPtr<String> &table_name() const { return table_name_; }
    SegmentID segment_id() const { return segment_id_; }
    BlockID block_id() const { return block_id_; }
    u16 row_count() const { return row_count_; }
    u16 row_capacity() const { return row_capacity_; }
    TxnTimeStamp min_row_ts() const { return min_row_ts_; }
    TxnTimeStamp max_row_ts() const { return max_row_ts_; }
    TxnTimeStamp checkpoint_ts() const { return checkpoint_ts_; }
    u16 checkpoint_row_count() const { return checkpoint_row_count_; }
    bool set_sealed() const { return set_sealed_; }
    const String &block_filter_binary_data() const { return block_filter_binary_data_; }

private:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SegmentID segment_id_{0};
    BlockID block_id_{0};

private:
    // For update
    u16 row_capacity_{0};
    u16 row_count_{0};
    TxnTimeStamp min_row_ts_{0};
    TxnTimeStamp max_row_ts_{0};
    TxnTimeStamp checkpoint_ts_{0};
    u16 checkpoint_row_count_{0};

    bool set_sealed_{false};
    String block_filter_binary_data_{};
};

/// class AddColumnEntryOp
export class AddColumnEntryOp : public CatalogDeltaOperation {
public:
    explicit AddColumnEntryOp(TxnTimeStamp begin_ts,
                              bool is_delete,
                              TransactionID txn_id,
                              TxnTimeStamp commit_ts,
                              SharedPtr<String> db_name,
                              SharedPtr<String> table_name,
                              SegmentID segment_id,
                              BlockID block_id,
                              ColumnID column_id,
                              i32 next_line_idx)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), segment_id_(segment_id), block_id_(block_id), column_id_(column_id), next_outline_idx_(next_line_idx) {}

    explicit AddColumnEntryOp(BlockColumnEntry *column_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY, column_entry),
          db_name_(column_entry->GetBlockEntry()->GetSegmentEntry()->GetTableEntry()->GetDBName()),
          table_name_(column_entry->GetBlockEntry()->GetSegmentEntry()->GetTableEntry()->GetTableName()),
          segment_id_(column_entry->GetBlockEntry()->GetSegmentEntry()->segment_id()), block_id_(column_entry->GetBlockEntry()->block_id()),
          column_id_(column_entry->column_id()), next_outline_idx_(column_entry->OutlineBufferCount()) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_COLUMN_ENTRY; }
    String GetTypeStr() const final { return "ADD_COLUMN_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(SegmentID) + sizeof(BlockID) + sizeof(ColumnID) + sizeof(i32);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("#{}#{}#{}#{}#{}", *db_name_, *table_name_, segment_id_, block_id_, column_id_));
    }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other) override;

public:
    const SharedPtr<String> &db_name() const { return db_name_; }
    const SharedPtr<String> &table_name() const { return table_name_; }
    SegmentID segment_id() const { return segment_id_; }
    BlockID block_id() const { return block_id_; }
    ColumnID column_id() const { return column_id_; }
    i32 next_outline_idx() const { return next_outline_idx_; }

private:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    ColumnID column_id_{};
    i32 next_outline_idx_{0};
};

/// class AddTableIndexEntryOp
export class AddTableIndexEntryOp : public CatalogDeltaOperation {
public:
    explicit AddTableIndexEntryOp(TxnTimeStamp begin_ts,
                                  bool is_delete,
                                  TransactionID txn_id,
                                  TxnTimeStamp commit_ts,
                                  SharedPtr<String> db_name,
                                  SharedPtr<String> table_name,
                                  SharedPtr<String> index_name,
                                  SharedPtr<String> index_dir,
                                  SharedPtr<IndexBase> index_base)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)), index_dir_(std::move(index_dir)),
          index_base_(std::move(index_base)) {}

    explicit AddTableIndexEntryOp(TableIndexEntry *table_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY, table_index_entry),
          db_name_(table_index_entry->table_index_meta()->GetTableEntry()->GetDBName()),
          table_name_(table_index_entry->table_index_meta()->GetTableEntry()->GetTableName()),
          index_name_(table_index_entry->table_index_meta()->index_name()), index_dir_(table_index_entry->index_dir()),
          index_base_(table_index_entry->table_index_def()) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_TABLE_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(i32) + this->index_name_->size();
        if (!is_delete()) {
            total_size += sizeof(i32) + this->index_dir_->size();
            total_size += this->index_base_->GetSizeInBytes();
        }
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}#{}#{}", *db_name_, *table_name_, *index_name_)); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other) override;

public:
    const SharedPtr<String> &db_name() const { return db_name_; }
    const SharedPtr<String> &table_name() const { return table_name_; }
    const SharedPtr<String> &index_name() const { return index_name_; }
    const SharedPtr<String> &index_dir() const { return index_dir_; }
    SharedPtr<IndexBase> index_base() const { return index_base_; }

private:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> index_name_{};
    SharedPtr<String> index_dir_{};
    SharedPtr<IndexBase> index_base_{};
};

/// class AddFulltextIndexEntryOp
export class AddFulltextIndexEntryOp : public CatalogDeltaOperation {
public:
    explicit AddFulltextIndexEntryOp(TxnTimeStamp begin_ts,
                                     bool is_delete,
                                     TransactionID txn_id,
                                     TxnTimeStamp commit_ts,
                                     SharedPtr<String> db_name,
                                     SharedPtr<String> table_name,
                                     SharedPtr<String> index_name)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)) {}

    explicit AddFulltextIndexEntryOp(FulltextIndexEntry *fulltext_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY, fulltext_index_entry),
          db_name_(fulltext_index_entry->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName()),
          table_name_(fulltext_index_entry->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName()),
          index_name_(fulltext_index_entry->table_index_entry()->table_index_meta()->index_name()) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_FULLTEXT_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_FULLTEXT_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(i32) + this->index_name_->size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}#{}#{}", *db_name_, *table_name_, *index_name_)); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;

public:
    const SharedPtr<String> &db_name() const { return db_name_; }
    const SharedPtr<String> &table_name() const { return table_name_; }
    const SharedPtr<String> &index_name() const { return index_name_; }
    void Merge(UniquePtr<CatalogDeltaOperation> other) override;

private:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> index_name_{};
};

/// class AddSegmentColumnEntryOperation
export class AddSegmentIndexEntryOp : public CatalogDeltaOperation {
public:
    explicit AddSegmentIndexEntryOp(TxnTimeStamp begin_ts,
                                    bool is_delete,
                                    TransactionID txn_id,
                                    TxnTimeStamp commit_ts,
                                    SharedPtr<String> db_name,
                                    SharedPtr<String> table_name,
                                    SharedPtr<String> index_name,
                                    SegmentID segment_id,
                                    TxnTimeStamp min_ts,
                                    TxnTimeStamp max_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY, begin_ts, is_delete, txn_id, commit_ts), db_name_(std::move(db_name)),
          table_name_(std::move(table_name)), index_name_(std::move(index_name)), segment_id_(segment_id), min_ts_(min_ts), max_ts_(max_ts) {}

    explicit AddSegmentIndexEntryOp(SegmentIndexEntry *segment_index_entry)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY, segment_index_entry), segment_index_entry_(segment_index_entry),
          db_name_(segment_index_entry->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName()),
          table_name_(segment_index_entry->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName()),
          index_name_(segment_index_entry->table_index_entry()->table_index_meta()->index_name()), segment_id_(segment_index_entry->segment_id()),
          min_ts_(segment_index_entry->min_ts()), max_ts_(segment_index_entry->max_ts()) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_SEGMENT_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(i32) + this->index_name_->size();
        total_size += sizeof(SegmentID) + sizeof(TxnTimeStamp) + sizeof(TxnTimeStamp);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}#{}#{}#{}", *db_name_, *table_name_, *index_name_, segment_id_)); }
    void Flush(TxnTimeStamp max_commit_ts);
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other) override;

public:
    const SharedPtr<String> &db_name() const { return db_name_; }
    const SharedPtr<String> &table_name() const { return table_name_; }
    const SharedPtr<String> &index_name() const { return index_name_; }
    SegmentID segment_id() const { return segment_id_; }
    TxnTimeStamp min_ts() const { return min_ts_; }
    TxnTimeStamp max_ts() const { return max_ts_; }

private:
    SegmentIndexEntry *segment_index_entry_{};

    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> index_name_{};
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

    static i32 GetSizeInBytes() { return sizeof(size_) + sizeof(checksum_); }
};

/// class CatalogDeltaEntry
export class CatalogDeltaEntry {
public:
    CatalogDeltaEntry() = default;
    [[nodiscard]] i32 GetSizeInBytes() const;

    void WriteAdv(char *&ptr);

    static SharedPtr<CatalogDeltaEntry> ReadAdv(char *&ptr, i32 max_bytes);

    [[nodiscard]] String ToString() const;

    void SaveState(TransactionID txn_id, TxnTimeStamp commit_ts);

    const Vector<TransactionID> &txn_ids() const { return txn_ids_; }
    void set_txn_ids(Vector<TransactionID> &&txn_ids) { txn_ids_ = std::move(txn_ids); }
    TxnTimeStamp commit_ts() const { return max_commit_ts_; }
    void set_commit_ts(TransactionID commit_ts) { max_commit_ts_ = commit_ts; }

    void AddOperation(UniquePtr<CatalogDeltaOperation> operation) { operations_.emplace_back(std::move(operation)); }

public:
    // Attention: only use in unit test or thread safe context
    Vector<UniquePtr<CatalogDeltaOperation>> &operations() { return operations_; }

private:
    std::mutex mtx_{};
    Vector<TransactionID> txn_ids_{};         // txn id of the entry
    TxnTimeStamp max_commit_ts_{UNCOMMIT_TS}; // commit timestamp of the txn

    Vector<UniquePtr<CatalogDeltaOperation>> operations_{};
};

export class GlobalCatalogDeltaEntry {
public:
    GlobalCatalogDeltaEntry() = default;

    void AddDeltaEntries(Vector<UniquePtr<CatalogDeltaEntry>> &&delta_entries);

    // Pick and remove all operations that are committed before `flush_ts`
    UniquePtr<CatalogDeltaEntry> PickFlushEntry(TxnTimeStamp flush_ts);

    SizeT OpSize() const { return delta_ops_.size(); }

private:
    bool PruneDeltaOp(CatalogDeltaOperation *delta_op, TxnTimeStamp current_commit_ts);

    void PruneOpWithSamePrefix(const String &prefix, TxnTimeStamp current_commit_ts);

private:
    std::mutex mtx_{};

    Map<String, UniquePtr<CatalogDeltaOperation>> delta_ops_;
    HashSet<TransactionID> txn_ids_;
    TransactionID max_commit_ts_{0};
};

} // namespace infinity

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

#include "type/complex/row_id.h"

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
import segment_index_entry;
import chunk_index_entry;
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
    ADD_SEGMENT_INDEX_ENTRY = 12,
    ADD_CHUNK_INDEX_ENTRY = 13,

    // -----------------------------
    // SEGMENT STATUS
    // -----------------------------
    SET_SEGMENT_STATUS_SEALED = 21,
    SET_BLOCK_STATUS_SEALED = 22,
};

export enum class MergeFlag : u8 {
    kInvalid = 0,
    kDelete = 1,
    kNew = 2,
    kUpdate = 3,
    kDeleteAndNew = 4, // this is used in catalog file, should not initialize in catalog delta
};

enum class PruneFlag : u8 {
    kInvalid = 0,
    kPrune = 1,
    kPruneSub = 2,
    kKeep = 3,
};

/// class CatalogDeltaOperation
export class CatalogDeltaOperation {
public:
    explicit CatalogDeltaOperation(CatalogDeltaOpType type) : type_(type) {}
    CatalogDeltaOperation(CatalogDeltaOpType type, BaseEntry *base_entry, TxnTimeStamp commit_ts);
    virtual ~CatalogDeltaOperation(){};
    virtual CatalogDeltaOpType GetType() const = 0;
    virtual String GetTypeStr() const = 0;
    [[nodiscard]] virtual SizeT GetSizeInBytes() const = 0;
    virtual void WriteAdv(char *&ptr) const = 0;
    static UniquePtr<CatalogDeltaOperation> ReadAdv(char *&ptr, i32 max_bytes);
    SizeT GetBaseSizeInBytes() const { return sizeof(TxnTimeStamp) + sizeof(merge_flag_) + sizeof(TransactionID) + sizeof(TxnTimeStamp); }
    void WriteAdvBase(char *&buf) const {
        WriteBufAdv(buf, this->type_);
        WriteBufAdv(buf, this->begin_ts_);
        WriteBufAdv(buf, this->merge_flag_);
        WriteBufAdv(buf, this->txn_id_);
        WriteBufAdv(buf, this->commit_ts_);
    }
    void ReadAdvBase(char *&ptr) {
        begin_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
        merge_flag_ = ReadBufAdv<MergeFlag>(ptr);
        txn_id_ = ReadBufAdv<TransactionID>(ptr);
        commit_ts_ = ReadBufAdv<TxnTimeStamp>(ptr);
    }

    virtual const String ToString() const {
        return fmt::format("type: {}, begin_ts: {}, txn_id: {}, commit_ts: {}, merge_flag: {}",
                           GetTypeStr(),
                           begin_ts_,
                           txn_id_,
                           commit_ts_,
                           (u8)merge_flag_);
    }
    virtual const String EncodeIndex() const = 0;
    virtual bool operator==(const CatalogDeltaOperation &rhs) const;
    virtual void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) = 0;

    static PruneFlag ToPrune(Optional<MergeFlag> old_merge_flag, MergeFlag new_merge_flag);

    MergeFlag NextDeleteFlag(MergeFlag new_merge_flag) const;

public:
    TxnTimeStamp begin_ts_{0};
    TransactionID txn_id_{0};
    TxnTimeStamp commit_ts_{0};
    MergeFlag merge_flag_{MergeFlag::kInvalid};

public:
    bool is_saved_sate_{false};
    CatalogDeltaOpType type_{CatalogDeltaOpType::INVALID};
};

/// class AddDBEntryOp
export class AddDBEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddDBEntryOp> ReadAdv(char *&ptr);

    AddDBEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_ENTRY) {}

    explicit AddDBEntryOp(DBEntry *db_entry, TxnTimeStamp commit_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_ENTRY, db_entry, commit_ts), db_name_(db_entry->db_name_ptr()),
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
    const String EncodeIndex() const final { return String(fmt::format("#{}@{}", *db_name_, (u8)(type_))); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) override;

public:
    SharedPtr<String> db_name_{};
    SharedPtr<String> db_entry_dir_{};
};

/// class AddTableEntryOp
export class AddTableEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddTableEntryOp> ReadAdv(char *&ptr, char *ptr_end);

    AddTableEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY) {}

    explicit AddTableEntryOp(TableEntry *table_entry, TxnTimeStamp commit_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY, table_entry, commit_ts), db_name_(table_entry->GetDBName()),
          table_name_(table_entry->GetTableName()), table_entry_dir_(table_entry->TableEntryDir()), column_defs_(table_entry->column_defs()),
          row_count_(table_entry->row_count()), // TODO: fix it
          unsealed_id_(table_entry->unsealed_id()), next_segment_id_(table_entry->next_segment_id()) {}

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
        total_size += sizeof(SegmentID) * 2;
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}#{}@{}", *db_name_, *table_name_, (u8)(type_))); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) override;

public:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> table_entry_dir_{};
    Vector<SharedPtr<ColumnDef>> column_defs_{};
    TableEntryType table_entry_type_{TableEntryType::kTableEntry};
    SizeT row_count_{0};
    SegmentID unsealed_id_{};
    SegmentID next_segment_id_{0};
};

/// class AddSegmentEntryOp
export class AddSegmentEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddSegmentEntryOp> ReadAdv(char *&ptr);

    AddSegmentEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_ENTRY){};

    explicit AddSegmentEntryOp(SegmentEntry *segment_entry, TxnTimeStamp commit_ts, String segment_filter_binary_data = "")
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_ENTRY, segment_entry, commit_ts),
          db_name_(segment_entry->GetTableEntry()->GetDBName()), table_name_(segment_entry->GetTableEntry()->GetTableName()),
          segment_id_(segment_entry->segment_id()), status_(segment_entry->status()), column_count_(segment_entry->column_count()),
          row_count_(segment_entry->row_count()),               // FIXME: use append_state
          actual_row_count_(segment_entry->actual_row_count()), // FIXME: use append_state
          row_capacity_(segment_entry->row_capacity()), min_row_ts_(segment_entry->min_row_ts()), max_row_ts_(segment_entry->max_row_ts()),
          deprecate_ts_(segment_entry->deprecate_ts()), segment_filter_binary_data_(std::move(segment_filter_binary_data)) {}

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
        total_size += sizeof(i32) + segment_filter_binary_data_.size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("#{}#{}#{}@{}", *this->db_name_, *this->table_name_, this->segment_id_, (u8)(type_)));
    }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) override;

public:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SegmentID segment_id_{};

public:
    SegmentStatus status_{};
    u64 column_count_{0};
    SizeT row_count_{0};
    SizeT actual_row_count_{0};
    SizeT row_capacity_{0};
    TxnTimeStamp min_row_ts_{0};
    TxnTimeStamp max_row_ts_{0};
    TxnTimeStamp deprecate_ts_{0};
    String segment_filter_binary_data_{};
};

/// class AddBlockEntryOp
export class AddBlockEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddBlockEntryOp> ReadAdv(char *&ptr);

    AddBlockEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_BLOCK_ENTRY){};

    explicit AddBlockEntryOp(BlockEntry *block_entry, TxnTimeStamp commit_ts, String block_filter_binary_data = "")
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_BLOCK_ENTRY, block_entry, commit_ts), block_entry_(block_entry),
          db_name_(block_entry->GetSegmentEntry()->GetTableEntry()->GetDBName()),
          table_name_(block_entry->GetSegmentEntry()->GetTableEntry()->GetTableName()), segment_id_(block_entry->GetSegmentEntry()->segment_id()),
          block_id_(block_entry->block_id()), row_capacity_(block_entry->row_capacity()), row_count_(block_entry->row_count()),
          min_row_ts_(block_entry->min_row_ts()), max_row_ts_(block_entry->max_row_ts()), checkpoint_ts_(block_entry->checkpoint_ts()),
          checkpoint_row_count_(block_entry->checkpoint_row_count()), block_filter_binary_data_(std::move(block_filter_binary_data)) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_BLOCK_ENTRY; }
    String GetTypeStr() const final { return "ADD_BLOCK_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(SegmentID) + sizeof(BlockID);
        total_size += sizeof(u16) + sizeof(u16) + sizeof(TxnTimeStamp) * 2;
        total_size += sizeof(TxnTimeStamp) + sizeof(u16);
        total_size += sizeof(i32) + this->block_filter_binary_data_.size();
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("#{}#{}#{}#{}@{}", *db_name_, *table_name_, segment_id_, block_id_, (u8)(type_)));
    }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) override;

    void FlushDataToDisk(TxnTimeStamp max_commit_ts);

public:
    BlockEntry *block_entry_{};

public:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SegmentID segment_id_{0};
    BlockID block_id_{0};

public:
    // For update
    u16 row_capacity_{0};
    u16 row_count_{0};
    TxnTimeStamp min_row_ts_{0};
    TxnTimeStamp max_row_ts_{0};
    TxnTimeStamp checkpoint_ts_{0};
    u16 checkpoint_row_count_{0};
    String block_filter_binary_data_{};
};

/// class AddColumnEntryOp
export class AddColumnEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddColumnEntryOp> ReadAdv(char *&ptr);

    AddColumnEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY){};

    explicit AddColumnEntryOp(BlockColumnEntry *column_entry, TxnTimeStamp commit_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY, column_entry, commit_ts),
          db_name_(column_entry->GetBlockEntry()->GetSegmentEntry()->GetTableEntry()->GetDBName()),
          table_name_(column_entry->GetBlockEntry()->GetSegmentEntry()->GetTableEntry()->GetTableName()),
          segment_id_(column_entry->GetBlockEntry()->GetSegmentEntry()->segment_id()), block_id_(column_entry->GetBlockEntry()->block_id()),
          column_id_(column_entry->column_id()), next_outline_idx_(column_entry->OutlineBufferCount()),
          last_chunk_offset_(column_entry->LastChunkOff()) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_COLUMN_ENTRY; }
    String GetTypeStr() const final { return "ADD_COLUMN_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(SegmentID) + sizeof(BlockID) + sizeof(ColumnID) + sizeof(i32) + sizeof(last_chunk_offset_);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("#{}#{}#{}#{}#{}@{}", *db_name_, *table_name_, segment_id_, block_id_, column_id_, (u8)(type_)));
    }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) override;

public:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SegmentID segment_id_{};
    BlockID block_id_{};
    ColumnID column_id_{};
    i32 next_outline_idx_{0};
    u64 last_chunk_offset_{};
};

/// class AddTableIndexEntryOp
export class AddTableIndexEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddTableIndexEntryOp> ReadAdv(char *&ptr, char *ptr_end);

    AddTableIndexEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY) {}

    explicit AddTableIndexEntryOp(TableIndexEntry *table_index_entry, TxnTimeStamp commit_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY, table_index_entry, commit_ts),
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
        if (merge_flag_ != MergeFlag::kDelete) {
            total_size += sizeof(i32) + this->index_dir_->size();
            total_size += this->index_base_->GetSizeInBytes();
        }
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final { return String(fmt::format("#{}#{}#{}@{}", *db_name_, *table_name_, *index_name_, (u8)(type_))); }
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) override;

public:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> index_name_{};
    SharedPtr<String> index_dir_{};
    SharedPtr<IndexBase> index_base_{};
};

/// class AddSegmentColumnEntryOperation
export class AddSegmentIndexEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddSegmentIndexEntryOp> ReadAdv(char *&ptr);

    AddSegmentIndexEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY) {}

    explicit AddSegmentIndexEntryOp(SegmentIndexEntry *segment_index_entry, TxnTimeStamp commit_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY, segment_index_entry, commit_ts),
          segment_index_entry_(segment_index_entry),
          db_name_(segment_index_entry->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName()),
          table_name_(segment_index_entry->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName()),
          index_name_(segment_index_entry->table_index_entry()->table_index_meta()->index_name()), segment_id_(segment_index_entry->segment_id()),
          min_ts_(segment_index_entry->min_ts()), max_ts_(segment_index_entry->max_ts()), next_chunk_id_(segment_index_entry->next_chunk_id()) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_SEGMENT_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(i32) + this->index_name_->size();
        total_size += sizeof(SegmentID) + sizeof(TxnTimeStamp) + sizeof(TxnTimeStamp);
        total_size += sizeof(ChunkID);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("#{}#{}#{}#{}@{}", *db_name_, *table_name_, *index_name_, segment_id_, (u8)(type_)));
    }
    void FlushDataToDisk(TxnTimeStamp max_commit_ts);
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) override;

public:
    SegmentIndexEntry *segment_index_entry_{};

    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> index_name_{};
    SegmentID segment_id_{0};
    TxnTimeStamp min_ts_{0};
    TxnTimeStamp max_ts_{0};
    ChunkID next_chunk_id_{0};
};

/// class AddSegmentColumnEntryOperation
export class AddChunkIndexEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddChunkIndexEntryOp> ReadAdv(char *&ptr);

    AddChunkIndexEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY) {}

    explicit AddChunkIndexEntryOp(ChunkIndexEntry *chunk_index_entry, TxnTimeStamp commit_ts)
        : CatalogDeltaOperation(CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY, chunk_index_entry, commit_ts),
          db_name_(chunk_index_entry->segment_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetDBName()),
          table_name_(chunk_index_entry->segment_index_entry_->table_index_entry()->table_index_meta()->GetTableEntry()->GetTableName()),
          index_name_(chunk_index_entry->segment_index_entry_->table_index_entry()->table_index_meta()->index_name()),
          segment_id_(chunk_index_entry->segment_index_entry_->segment_id()), chunk_id_(chunk_index_entry->chunk_id_),
          base_name_(chunk_index_entry->base_name_), base_rowid_(chunk_index_entry->base_rowid_), row_count_(chunk_index_entry->row_count_) {}

    CatalogDeltaOpType GetType() const final { return CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY; }
    String GetTypeStr() const final { return "ADD_CHUNK_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final {
        auto total_size = sizeof(CatalogDeltaOpType) + GetBaseSizeInBytes();
        total_size += sizeof(i32) + this->db_name_->size();
        total_size += sizeof(i32) + this->table_name_->size();
        total_size += sizeof(i32) + this->index_name_->size();
        total_size += sizeof(SegmentID);
        total_size += sizeof(ChunkID);
        total_size += sizeof(i32) + this->base_name_.size();
        total_size += sizeof(RowID);
        total_size += sizeof(u32);
        return total_size;
    }
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    const String EncodeIndex() const final {
        return String(fmt::format("#{}#{}#{}#{}#{}@{}", *db_name_, *table_name_, *index_name_, segment_id_, chunk_id_, (u8)(type_)));
    }
    void Flush(TxnTimeStamp max_commit_ts);
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(UniquePtr<CatalogDeltaOperation> other, TxnTimeStamp last_full_checkpoint_ts) override;

public:
    SharedPtr<String> db_name_{};
    SharedPtr<String> table_name_{};
    SharedPtr<String> index_name_{};
    SegmentID segment_id_{0};
    ChunkID chunk_id_{};
    String base_name_{};
    RowID base_rowid_;
    u32 row_count_{0};
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

    static UniquePtr<CatalogDeltaEntry> ReadAdv(char *&ptr, i32 max_bytes);

    [[nodiscard]] String ToString() const;

    void SaveState(TransactionID txn_id, TxnTimeStamp commit_ts, u64 sequence);

    const Vector<TransactionID> &txn_ids() const { return txn_ids_; }
    void set_txn_ids(Vector<TransactionID> &&txn_ids) { txn_ids_ = std::move(txn_ids); }
    TxnTimeStamp commit_ts() const { return max_commit_ts_; }
    void set_commit_ts(TransactionID commit_ts) { max_commit_ts_ = commit_ts; }
    u64 sequence() const { return sequence_; }

    void AddOperation(UniquePtr<CatalogDeltaOperation> operation);

public:
    // Attention: only use in unit test or thread safe context
    Vector<UniquePtr<CatalogDeltaOperation>> &operations() { return operations_; }

private:
    Vector<TransactionID> txn_ids_{};         // txn id of the entry
    TxnTimeStamp max_commit_ts_{UNCOMMIT_TS}; // commit timestamp of the txn
    u64 sequence_{};
    Vector<UniquePtr<CatalogDeltaOperation>> operations_{};
};

export class GlobalCatalogDeltaEntry {
public:
    GlobalCatalogDeltaEntry() = default;

    // Must be called before any checkpoint.
    void InitMaxCommitTS(TxnTimeStamp max_commit_ts) { max_commit_ts_ = max_commit_ts; }

    void InitFullCheckpointTs(TxnTimeStamp last_full_ckp_ts) { last_full_ckp_ts_ = last_full_ckp_ts; }

    void AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry, i64 wal_size);

    void ReplayDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry);

    // Pick and remove all operations that are committed before `max_commit_ts`, after `full_ckp_ts`
    UniquePtr<CatalogDeltaEntry> PickFlushEntry(TxnTimeStamp full_ckp_ts, TxnTimeStamp max_commit_ts);

    Tuple<TxnTimeStamp, i64> GetCheckpointState() const { return {max_commit_ts_, wal_size_}; }

    SizeT OpSize() const { return delta_ops_.size(); }

private:
    void AddDeltaEntryInner(CatalogDeltaEntry *delta_entry);

    void PruneOpWithSamePrefix(const String &prefix);

private:
    u64 last_sequence_{0};
    std::priority_queue<u64, Vector<u64>, std::greater<u64>> sequence_heap_;
    Map<u64, UniquePtr<CatalogDeltaEntry>> delta_entry_map_;

    Map<String, UniquePtr<CatalogDeltaOperation>> delta_ops_;
    HashSet<TransactionID> txn_ids_;
    // update by add delta entry, read by bg_process::checkpoint
    TxnTimeStamp max_commit_ts_{0};
    TxnTimeStamp last_full_ckp_ts_{0};
    i64 wal_size_{};
};

} // namespace infinity

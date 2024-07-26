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
import constant_expr;
import create_index_info;
import persistence_manager;

namespace infinity {

export struct FullTextObjAddrs {
    ObjAddr posting_obj_addr_;
    ObjAddr dict_obj_addr_;
    ObjAddr column_length_obj_addr_;

    void Save(const ObjAddr &posting_obj_addr, const ObjAddr &dict_obj_addr, const ObjAddr &column_length_obj_addr) {
        posting_obj_addr_ = posting_obj_addr;
        dict_obj_addr_ = dict_obj_addr;
        column_length_obj_addr_ = column_length_obj_addr;
    }

    void ToVec(Vector<ObjAddr> &obj_addrs) {
        obj_addrs.push_back(posting_obj_addr_);
        obj_addrs.push_back(dict_obj_addr_);
        obj_addrs.push_back(column_length_obj_addr_);
    }
};

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

export String ToString(CatalogDeltaOpType op_type);

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
    virtual ~CatalogDeltaOperation() {};
    CatalogDeltaOpType GetType() const { return type_; }
    virtual String GetTypeStr() const = 0;
    [[nodiscard]] virtual SizeT GetSizeInBytes() const = 0;
    virtual void WriteAdv(char *&ptr) const = 0;
    static UniquePtr<CatalogDeltaOperation> ReadAdv(char *&ptr, i32 max_bytes);
    SizeT GetBaseSizeInBytes() const;
    void WriteAdvBase(char *&buf) const;
    void ReadAdvBase(char *&ptr);

    virtual const String ToString() const;
    virtual bool operator==(const CatalogDeltaOperation &rhs) const;
    virtual void Merge(CatalogDeltaOperation &other) = 0;

    static PruneFlag ToPrune(Optional<MergeFlag> old_merge_flag, MergeFlag new_merge_flag);

    MergeFlag NextDeleteFlag(MergeFlag new_merge_flag) const;

public:
    TxnTimeStamp begin_ts_{0};
    TransactionID txn_id_{0};
    TxnTimeStamp commit_ts_{0};
    MergeFlag merge_flag_{MergeFlag::kInvalid};
    SharedPtr<String> encode_;

public:
    CatalogDeltaOpType type_{CatalogDeltaOpType::INVALID};
};

/// class AddDBEntryOp
export class AddDBEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddDBEntryOp> ReadAdv(char *&ptr);

    AddDBEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_DATABASE_ENTRY) {}

    AddDBEntryOp(DBEntry *db_entry, TxnTimeStamp commit_ts);

    String GetTypeStr() const final { return "ADD_DATABASE_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(CatalogDeltaOperation &other) override;

public:
    SharedPtr<String> db_entry_dir_{};
};

/// class AddTableEntryOp
export class AddTableEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddTableEntryOp> ReadAdv(char *&ptr, char *ptr_end);

    AddTableEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_ENTRY) {}

    AddTableEntryOp(TableEntry *table_entry, TxnTimeStamp commit_ts);

    String GetTypeStr() const final { return "ADD_TABLE_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(CatalogDeltaOperation &other) override;

public:
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

    AddSegmentEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_ENTRY) {};

    AddSegmentEntryOp(SegmentEntry *segment_entry, TxnTimeStamp commit_ts, String segment_filter_binary_data = "");

    String GetTypeStr() const final { return "ADD_SEGMENT_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(CatalogDeltaOperation &other) override;

public:
    SegmentStatus status_{};
    u64 column_count_{0};
    SizeT row_count_{0};
    SizeT actual_row_count_{0};
    SizeT row_capacity_{0};
    TxnTimeStamp min_row_ts_{0};
    TxnTimeStamp max_row_ts_{0};
    TxnTimeStamp first_delete_ts_{UNCOMMIT_TS};
    TxnTimeStamp deprecate_ts_{0};
    String segment_filter_binary_data_{};
};

/// class AddBlockEntryOp
export class AddBlockEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddBlockEntryOp> ReadAdv(char *&ptr);

    AddBlockEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_BLOCK_ENTRY) {};

    AddBlockEntryOp(BlockEntry *block_entry, TxnTimeStamp commit_ts, String block_filter_binary_data = "");

    String GetTypeStr() const final { return "ADD_BLOCK_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(CatalogDeltaOperation &other) override;

    void FlushDataToDisk(TxnTimeStamp max_commit_ts);

public:
    BlockEntry *block_entry_{};

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

    AddColumnEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_COLUMN_ENTRY) {};

    AddColumnEntryOp(BlockColumnEntry *column_entry, TxnTimeStamp commit_ts);

    String GetTypeStr() const final { return "ADD_COLUMN_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(CatalogDeltaOperation &other) override;

public:
    Vector<Pair<u32, u64>> outline_infos_;
};

/// class AddTableIndexEntryOp
export class AddTableIndexEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddTableIndexEntryOp> ReadAdv(char *&ptr, char *ptr_end);

    AddTableIndexEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_TABLE_INDEX_ENTRY) {}

    AddTableIndexEntryOp(TableIndexEntry *table_index_entry, TxnTimeStamp commit_ts);

    String GetTypeStr() const final { return "ADD_TABLE_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(CatalogDeltaOperation &other) override;

public:
    SharedPtr<String> index_dir_{};
    SharedPtr<IndexBase> index_base_{};
};

/// class AddSegmentColumnEntryOperation
export class AddSegmentIndexEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddSegmentIndexEntryOp> ReadAdv(char *&ptr);

    AddSegmentIndexEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_SEGMENT_INDEX_ENTRY) {}

    AddSegmentIndexEntryOp(SegmentIndexEntry *segment_index_entry, TxnTimeStamp commit_ts);

    String GetTypeStr() const final { return "ADD_SEGMENT_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    void FlushDataToDisk(TxnTimeStamp max_commit_ts);
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(CatalogDeltaOperation &other) override;

public:
    SegmentIndexEntry *segment_index_entry_{};

    TxnTimeStamp min_ts_{0};
    TxnTimeStamp max_ts_{0};
    ChunkID next_chunk_id_{0};
};

/// class AddSegmentColumnEntryOperation
export class AddChunkIndexEntryOp : public CatalogDeltaOperation {
public:
    static UniquePtr<AddChunkIndexEntryOp> ReadAdv(char *&ptr);

    AddChunkIndexEntryOp() : CatalogDeltaOperation(CatalogDeltaOpType::ADD_CHUNK_INDEX_ENTRY) {}

    AddChunkIndexEntryOp(ChunkIndexEntry *chunk_index_entry, TxnTimeStamp commit_ts);

    String GetTypeStr() const final { return "ADD_CHUNK_INDEX_ENTRY"; }
    [[nodiscard]] SizeT GetSizeInBytes() const final;
    void WriteAdv(char *&buf) const final;
    const String ToString() const final;
    void Flush(TxnTimeStamp max_commit_ts);
    bool operator==(const CatalogDeltaOperation &rhs) const override;
    void Merge(CatalogDeltaOperation &other) override;

public:
    String base_name_{};
    IndexType index_type_{IndexType::kInvalid};
    FullTextObjAddrs fulltext_obj_addrs_{};
    RowID base_rowid_;
    u32 row_count_{0};
    TxnTimeStamp deprecate_ts_{UNCOMMIT_TS};
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

export struct CatalogDeltaOpBrief {
    TxnTimeStamp begin_ts_{0};
    TxnTimeStamp commit_ts_{0};
    TransactionID txn_id_{0};
    CatalogDeltaOpType type_{CatalogDeltaOpType::INVALID};
    SharedPtr<String> text_ptr;
};

export class GlobalCatalogDeltaEntry {
public:
    GlobalCatalogDeltaEntry() = default;

    void InitFullCheckpointTs(TxnTimeStamp last_full_ckp_ts);

    void AddDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry);

    void ReplayDeltaEntry(UniquePtr<CatalogDeltaEntry> delta_entry);

    // Pick and remove all operations that are committed before `max_commit_ts`, after `full_ckp_ts`
    UniquePtr<CatalogDeltaEntry> PickFlushEntry(TxnTimeStamp max_commit_ts);

    Vector<CatalogDeltaOpBrief> GetOperationBriefs() const;

    SizeT OpSize() const;

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
    TxnTimeStamp last_full_ckp_ts_{0};

    mutable std::mutex catalog_delta_locker_{};
};

} // namespace infinity

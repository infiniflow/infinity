//
// Created by jinhai on 23-6-23.
//

#pragma once

#include "base_entry.h"
#include "json.hpp"
#include "parser/statement/extra/extra_ddl_info.h"
#include "storage/meta/entry/table_collecton_type.h"

namespace infinity {

class TxnManager;

class DBEntry;
class TableCollectionMeta;
class BufferManager;
class Txn;
class BlockIndex;
class ColumnDef;
class DeleteState;
class ScanState;
class IndexMeta;
class SegmentEntry;
class IndexDef;
class AppendState;

struct TableCollectionEntry : public BaseEntry {
public:
    explicit TableCollectionEntry(const SharedPtr<String> &db_entry_dir,
                                  SharedPtr<String> table_collection_name,
                                  const Vector<SharedPtr<ColumnDef>> &columns,
                                  TableCollectionType table_collection_type,
                                  TableCollectionMeta *table_collection_meta,
                                  u64 txn_id,
                                  TxnTimeStamp begin_ts);

public:
    static EntryResult CreateIndex(TableCollectionEntry *table_entry,
                                   SharedPtr<IndexDef> index_def,
                                   ConflictType conflict_type,
                                   u64 txn_id,
                                   TxnTimeStamp begin_ts,
                                   TxnManager *txn_mgr);

    static EntryResult DropIndex(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult GetIndex(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnTimeStamp begin_ts);

    static EntryResult RemoveIndex(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnManager *txn_mgr);

public:
    static void Append(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store, BufferManager *buffer_mgr);

    static UniquePtr<String> Delete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &delete_state, BufferManager *buffer_mgr);

    static UniquePtr<String> InitScan(TableCollectionEntry *table_entry, Txn *txn_ptr, ScanState &scan_state, BufferManager *buffer_mgr);

    static UniquePtr<String> Scan(TableCollectionEntry *table_entry, Txn *txn_ptr, const ScanState &scan_state, BufferManager *buffer_mgr);

    static void CommitAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, const AppendState *append_state_ptr, BufferManager *buffer_mgr);

    static void RollbackAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store);

    static UniquePtr<String> CommitDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &append_state, BufferManager *buffer_mgr);

    static UniquePtr<String> RollbackDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &append_state, BufferManager *buffer_mgr);

    static UniquePtr<String> ImportAppendSegment(TableCollectionEntry *table_entry,
                                                 Txn *txn_ptr,
                                                 SharedPtr<SegmentEntry> segment,
                                                 AppendState &append_state,
                                                 BufferManager *buffer_mgr);

    static inline u64 GetNextSegmentID(TableCollectionEntry *table_entry) { return table_entry->next_segment_id_++; }

    static inline u64 GetMaxSegmentID(const TableCollectionEntry *table_entry) { return table_entry->next_segment_id_; }

    static SegmentEntry *GetSegmentByID(const TableCollectionEntry *table_entry, u64 id);

    static DBEntry *GetDBEntry(const TableCollectionEntry *table_entry);

    inline static TableCollectionMeta *GetTableMeta(const TableCollectionEntry *table_entry) {
        return (TableCollectionMeta *)table_entry->table_collection_meta_;
    }

    static SharedPtr<BlockIndex> GetBlockIndex(TableCollectionEntry *table_collection_entry, u64 txn_id, TxnTimeStamp begin_ts);

    static nlohmann::json Serialize(const TableCollectionEntry *table_entry);

    static UniquePtr<TableCollectionEntry>
    Deserialize(const nlohmann::json &table_entry_json, TableCollectionMeta *table_meta, BufferManager *buffer_mgr);

public:
    u64 GetColumnIdByName(const String &column_name);

private:
    HashMap<String, u64> name2id_;

public:
    RWMutex rw_locker_{};

    SharedPtr<String> table_entry_dir_{};

    SharedPtr<String> table_collection_name_{};
    Vector<SharedPtr<ColumnDef>> columns_{}; // 2. So this should be HashMap
    TableCollectionType table_collection_type_{TableCollectionType::kTableEntry};

    TableCollectionMeta *table_collection_meta_{};

    // from data table
    SizeT row_count_{};
    HashMap<u64, SharedPtr<SegmentEntry>> segments_{};
    SegmentEntry *unsealed_segment_{};
    au64 next_segment_id_{};

    HashMap<String, UniquePtr<IndexMeta>> indexes_{};
};

} // namespace infinity

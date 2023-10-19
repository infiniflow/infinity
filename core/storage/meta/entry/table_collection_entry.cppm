//
// Created by jinhai on 23-6-23.
//

module;

import stl;
import parser;
import txn;
import buffer_manager;
import third_party;
import table_collection_type;
import index_def;
import base_entry;
import segment_entry;
import block_index;

export module table_collection_entry;

namespace infinity {

class DBEntry;
class TableCollectionMeta;
//class BufferManager;
//class Txn;
//class BlockIndex;
//class ColumnDef;
class DeleteState;
class ScanState;
class AppendState;

export struct TableCollectionEntry : public BaseEntry {
public:
    explicit TableCollectionEntry(const SharedPtr<String> &db_entry_dir,
                                  SharedPtr<String> table_collection_name,
                                  const Vector<SharedPtr<ColumnDef>> &columns,
                                  TableCollectionType table_collection_type,
                                  TableCollectionMeta *table_collection_meta,
                                  u64 txn_id,
                                  TxnTimeStamp begin_ts);

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

    static EntryResult CreateIndex(TableCollectionEntry *table_entry, Txn *txn_ptr, SharedPtr<IndexDef> index_def);

    static inline u64 GetNextSegmentID(TableCollectionEntry *table_entry) { return table_entry->next_segment_id_++; }

    static inline u64 GetMaxSegmentID(const TableCollectionEntry *table_entry) { return table_entry->next_segment_id_; }

    static SegmentEntry *GetSegmentByID(const TableCollectionEntry *table_entry, u64 id);

    static DBEntry *GetDBEntry(const TableCollectionEntry *table_entry);

    inline static TableCollectionMeta *GetTableMeta(const TableCollectionEntry *table_entry) {
        return (TableCollectionMeta *)table_entry->table_collection_meta_;
    }

    static SharedPtr<BlockIndex> GetBlockIndex(TableCollectionEntry *table_collection_entry, u64 txn_id, TxnTimeStamp begin_ts);

    static Json Serialize(const TableCollectionEntry *table_entry);

    static UniquePtr<TableCollectionEntry>
    Deserialize(const Json &table_entry_json, TableCollectionMeta *table_meta, BufferManager *buffer_mgr);

public:
    u64 GetColumnIdByName(const String &column_name);

private:
    HashMap<String, u64> name2id_;

public:
    RWMutex rw_locker_{};

    SharedPtr<String> table_entry_dir_{};

    SharedPtr<String> table_collection_name_{};
    Vector<SharedPtr<ColumnDef>> columns_{};
    TableCollectionType table_collection_type_{TableCollectionType::kTableEntry};

    TableCollectionMeta *table_collection_meta_{};

    // from data table
    SizeT row_count_{};
    HashMap<u64, SharedPtr<SegmentEntry>> segments_{};
    SegmentEntry *unsealed_segment_{};
    au64 next_segment_id_{};

    HashMap<String, SharedPtr<IndexDef>> indexes_{};
};

} // namespace infinity

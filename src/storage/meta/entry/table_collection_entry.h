//
// Created by jinhai on 23-6-23.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/table_def.h"
#include "storage/table/data_table.h"
#include "segment_entry.h"
#include "table_collecton_type.h"

namespace infinity {

class DBEntry;
class TableCollectionMeta;
class BufferManager;
class Txn;

struct TableCollectionEntry : public BaseEntry {
public:
    explicit
    TableCollectionEntry(const SharedPtr<String>& base_dir,
                         SharedPtr<String> table_collection_name,
                         const Vector<SharedPtr<ColumnDef>>& columns,
                         TableCollectionType table_collection_type,
                         TableCollectionMeta* table_collection_meta,
                         u64 txn_id,
                         TxnTimeStamp begin_ts);

public:
    static void
    Append(TableCollectionEntry* table_entry, Txn* txn_ptr, void* txn_store, BufferManager* buffer_mgr);

    static UniquePtr<String>
    Delete(TableCollectionEntry* table_entry, Txn* txn_ptr, DeleteState& delete_state, BufferManager* buffer_mgr);

    static UniquePtr<String>
    InitScan(TableCollectionEntry* table_entry, Txn* txn_ptr, ScanState& scan_state, BufferManager* buffer_mgr);

    static UniquePtr<String>
    Scan(TableCollectionEntry* table_entry, Txn* txn_ptr, ScanState scan_state, BufferManager* buffer_mgr);

    static void
    CommitAppend(TableCollectionEntry* table_entry, Txn* txn_ptr, const AppendState* append_state_ptr, BufferManager* buffer_mgr);

    static void
    RollbackAppend(TableCollectionEntry* table_entry, Txn* txn_ptr, void* txn_store);

    static UniquePtr<String>
    CommitDelete(TableCollectionEntry* table_entry, Txn* txn_ptr, DeleteState& append_state, BufferManager* buffer_mgr);

    static UniquePtr<String>
    RollbackDelete(TableCollectionEntry* table_entry, Txn* txn_ptr, DeleteState& append_state, BufferManager* buffer_mgr);

    static UniquePtr<String>
    ImportAppendSegment(TableCollectionEntry* table_entry,
                        Txn* txn_ptr,
                        SharedPtr<SegmentEntry> segment,
                        AppendState& append_state,
                        BufferManager* buffer_mgr);

    static inline u64
    GetNextSegmentID(TableCollectionEntry* table_entry) {
        return table_entry->next_segment_id_ ++;
    }

    static inline u64
    GetMaxSegmentID(const TableCollectionEntry* table_entry) {
        return table_entry->next_segment_id_;
    }

    static inline SegmentEntry*
    GetSegmentByID(TableCollectionEntry* table_entry, u64 id) {
        return table_entry->segments_[id].get();
    }

    static DBEntry*
    GetDBEntry(const TableCollectionEntry* table_entry);

    inline static TableCollectionMeta*
    GetTableMeta(const TableCollectionEntry* table_entry) {
        return (TableCollectionMeta*)table_entry->table_collection_meta_;
    }

    static nlohmann::json
    Serialize(const TableCollectionEntry* table_entry);

    static UniquePtr<TableCollectionEntry>
    Deserialize(const nlohmann::json& table_entry_json,
                TableCollectionMeta* table_meta,
                BufferManager* buffer_mgr);

public:
    u64
    GetColumnIdByName(const String& column_name);

private:
    HashMap<String, u64> name2id_;

public:
    RWMutex rw_locker_{};

    SharedPtr<String> base_dir_{};

    SharedPtr<String> table_collection_name_{};
    Vector<SharedPtr<ColumnDef>> columns_{};
    TableCollectionType table_collection_type_{TableCollectionType::kTableEntry};

    TableCollectionMeta* table_collection_meta_{};

    // from data table
    SizeT row_count_{};
    HashMap<u64, SharedPtr<SegmentEntry>> segments_{};
    SegmentEntry* unsealed_segment_{};
    au64 next_segment_id_{};

    // Reserved
//    SharedPtr<DataTable> data_table_{};
};

}

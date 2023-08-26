//
// Created by jinhai on 23-6-23.
//

#pragma once

#include <utility>

#include "db_entry.h"
#include "common/types/internal_types.h"
#include "storage/table_def.h"
#include "storage/table/data_table.h"
#include "segment_entry.h"

namespace infinity {

class DBEntry;
class TableMeta;
class BufferManager;
class Txn;

struct TableEntry : public BaseEntry {
public:
    explicit
    TableEntry(const SharedPtr<String>& base_dir,
               SharedPtr<String> table_name,
               const Vector<SharedPtr<ColumnDef>>& columns,
               TableMeta* table_meta,
               u64 txn_id,
               TxnTimeStamp begin_ts)
            : BaseEntry(EntryType::kTable),
              base_dir_(base_dir), table_name_(std::move(table_name)), columns_(columns), table_meta_(table_meta) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

public:
    static void
    Append(TableEntry* table_entry, Txn* txn_ptr, void* txn_store, BufferManager* buffer_mgr);

    static UniquePtr<String>
    Delete(TableEntry* table_entry, Txn* txn_ptr, DeleteState& delete_state, BufferManager* buffer_mgr);

    static UniquePtr<String>
    InitScan(TableEntry* table_entry, Txn* txn_ptr, ScanState& scan_state, BufferManager* buffer_mgr);

    static UniquePtr<String>
    Scan(TableEntry* table_entry, Txn* txn_ptr, ScanState scan_state, BufferManager* buffer_mgr);

    static void
    CommitAppend(TableEntry* table_entry, Txn* txn_ptr, const AppendState* append_state_ptr, BufferManager* buffer_mgr);

    static void
    RollbackAppend(TableEntry* table_entry, Txn* txn_ptr, void* txn_store);

    static UniquePtr<String>
    CommitDelete(TableEntry* table_entry, Txn* txn_ptr, DeleteState& append_state, BufferManager* buffer_mgr);

    static UniquePtr<String>
    RollbackDelete(TableEntry* table_entry, Txn* txn_ptr, DeleteState& append_state, BufferManager* buffer_mgr);

    static UniquePtr<String>
    ImportAppendSegment(TableEntry* table_entry,
                        Txn* txn_ptr,
                        SharedPtr<SegmentEntry> segment,
                        AppendState& append_state,
                        BufferManager* buffer_mgr);

    static inline u64
    GetNextSegmentID(TableEntry* table_entry) {
        return table_entry->next_segment_id_ ++;
    }

    static inline u64
    GetMaxSegmentID(const TableEntry* table_entry) {
        return table_entry->next_segment_id_;
    }

    static inline SegmentEntry*
    GetSegmentByID(TableEntry* table_entry, u64 id) {
        return table_entry->segments_[id].get();
    }

    static DBEntry*
    GetDBEntry(const TableEntry* table_entry);

    inline static TableMeta*
    GetTableMeta(const TableEntry* table_entry) {
        return (TableMeta*)table_entry->table_meta_;
    }

    static nlohmann::json
    Serialize(const TableEntry* table_entry);

    static UniquePtr<TableEntry>
    Deserialize(const nlohmann::json& table_entry_json,
                TableMeta* table_meta,
                BufferManager* buffer_mgr);

public:
    RWMutex rw_locker_{};

    SharedPtr<String> base_dir_{};

    SharedPtr<String> table_name_{};
    Vector<SharedPtr<ColumnDef>> columns_{};

    TableMeta* table_meta_{};

    // from data table
    SizeT row_count_{};
    HashMap<u64, SharedPtr<SegmentEntry>> segments_{};
    SegmentEntry* unsealed_segment_{};
    au64 next_segment_id_{};

    // Reserved
//    SharedPtr<DataTable> data_table_{};
};

}

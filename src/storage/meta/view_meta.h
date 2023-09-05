//
// Created by jinhai on 23-9-3.
//


#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/meta/entry/base_entry.h"
#include "storage/table_def.h"

namespace infinity {

class TxnManager;
class DBEntry;
class BufferManager;
struct ViewMeta {
public:
    explicit
    ViewMeta(SharedPtr<String> name,
             DBEntry* db_entry) : view_name_(std::move(name)), db_entry_(db_entry) {
    }

public:
    static EntryResult
    CreateNewEntry(ViewMeta* table_meta,
                   const SharedPtr<String>& view_name,
                   const Vector<SharedPtr<ColumnDef>>& columns,
                   u64 txn_id,
                   TxnTimeStamp begin_ts,
                   TxnManager* txn_mgr);

    static EntryResult
    DropNewEntry(ViewMeta* table_meta,
                 u64 txn_id,
                 TxnTimeStamp begin_ts,
                 TxnManager* txn_mgr,
                 const String& table_name);

    static void
    DeleteNewEntry(ViewMeta* table_meta,
                   u64 txn_id,
                   TxnManager* txn_mgr);

    static EntryResult
    GetEntry(ViewMeta* table_meta,
             u64 txn_id,
             TxnTimeStamp begin_ts);

    static SharedPtr<String>
    ToString(ViewMeta* table_meta);

    static inline void*
    GetDBEntry(ViewMeta* view_meta) {
        return view_meta->db_entry_;
    }

public:
    RWMutex rw_locker_{};
    SharedPtr<String> view_name_{};

    DBEntry* db_entry_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

}

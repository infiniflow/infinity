//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "base_entry.h"
#include "common/types/internal_types.h"
#include "table_entry.h"
#include "storage/meta/table_meta.h"

namespace infinity {

class DBEntry : public BaseEntry {
public:
    explicit
    DBEntry(String db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context)
        : BaseEntry(EntryType::kDatabase, txn_context), db_name_(std::move(db_name)) {
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    EntryResult
    CreateTable(UniquePtr<TableDef> table_def,
                u64 txn_id,
                TxnTimeStamp begin_ts,
                TxnContext* txn_context);

    EntryResult
    DropTable(const String& table_name, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context);

    EntryResult
    GetTable(const String& table_name, u64 txn_id, TxnTimeStamp begin_ts);

    void
    RemoveTableEntry(const String& table_name, u64 txn_id, TxnContext* txn_context);

private:
    RWMutex rw_locker_{};
    String db_name_{};
    HashMap<String, UniquePtr<TableMeta>> tables_{};
};

}

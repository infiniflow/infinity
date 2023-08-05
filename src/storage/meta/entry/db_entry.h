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
    DBEntry(const SharedPtr<String>& dir,
            String db_name, u64 txn_id,
            TxnTimeStamp begin_ts,
            TxnContext* txn_context,
            void* buffer_mgr)
        : BaseEntry(EntryType::kDatabase, txn_context), db_name_(std::move(db_name)), buffer_mgr_(buffer_mgr) {
        dir_ = MakeShared<String>(*dir + "/txn_" + std::to_string(txn_id));
        begin_ts_ = begin_ts;
        txn_id_ = txn_id;
    }

    EntryResult
    CreateTable(const SharedPtr<TableDef>& table_def,
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
    SharedPtr<String> dir_{};
    String db_name_{};
    void* buffer_mgr_{};
    HashMap<String, UniquePtr<TableMeta>> tables_{};
};

}

//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/meta/entry/db_entry.h"
#include "storage/transaction/txn_context.h"

namespace infinity {

class DBMeta {
public:
    explicit
    DBMeta(String name) : db_name_(std::move(name)) {}

    EntryResult
    CreateNewEntry(u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context);

    EntryResult
    DropNewEntry(u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context);

    void
    DeleteNewEntry(u64 txn_id, TxnContext* txn_context);

    EntryResult
    GetEntry(u64 txn_id, TxnTimeStamp begin_ts);

private:
    RWMutex rw_locker_{};
    String db_name_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

}


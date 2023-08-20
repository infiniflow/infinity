//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/meta/entry/db_entry.h"
#include "storage/txn/txn_context.h"

namespace infinity {

struct DBMeta {
public:
    explicit
    DBMeta(const SharedPtr<String>& base_dir,
           String name)
           : db_name_(std::move(name)), base_dir_(base_dir) {
    }

public:

    // Reserved
    static EntryResult
    CreateNewEntry(DBMeta* db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context);

    static EntryResult
    DropNewEntry(DBMeta* db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context);

    static void
    DeleteNewEntry(DBMeta* db_meta, u64 txn_id, TxnContext* txn_context);

    static EntryResult
    GetEntry(DBMeta* db_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<String>
    ToString(DBMeta* db_meta);

public:
    RWMutex rw_locker_{};
    String db_name_{};
    SharedPtr<String> base_dir_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

}


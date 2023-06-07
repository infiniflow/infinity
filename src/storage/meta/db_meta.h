//
// Created by jinhai on 23-6-4.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "storage/meta/entry/db_entry.h"

namespace infinity {

class DBMeta {
public:
    explicit
    DBMeta(String name) : db_name_(std::move(name)) {}

    DBEntry*
    CreateNewEntry(u64 txn_id, TxnTimeStamp begin_ts);

private:
    RWMutex rw_locker_{};
    String db_name_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

}


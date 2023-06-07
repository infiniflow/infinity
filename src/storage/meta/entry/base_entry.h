//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/transaction/constants.h"

namespace infinity {

enum class EntryType {
    kInvalid,
    kDatabase,
    kTable,
};

struct BaseEntry {
    explicit
    BaseEntry(EntryType entry_type) : entry_type_(entry_type) {}

    virtual
    ~BaseEntry() = default;

    std::atomic_uint64_t txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    TxnTimeStamp commit_ts_{UNCOMMIT_TS};

    bool deleted_{false};

    EntryType entry_type_{EntryType::kInvalid};
};

}

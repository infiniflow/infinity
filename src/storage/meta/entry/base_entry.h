//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/txn/constants.h"
#include "storage/txn/txn_context.h"

namespace infinity {

enum class EntryType {
    kDummy,
    kDatabase,
    kTable,
    kBlock,
};

struct BaseEntry {
    explicit
    BaseEntry(EntryType entry_type, TxnContext* txn_context) : entry_type_(entry_type), txn_context_(txn_context) {}

    virtual
    ~BaseEntry() = default;

    inline void
    Commit(TxnTimeStamp commit_ts) {
        commit_ts_.store(commit_ts);
    }

    [[nodiscard]] inline bool
    Committed() const {
        return commit_ts_ != UNCOMMIT_TS;
    }

    TxnContext *txn_context_;
    std::atomic_uint64_t txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    std::atomic<TxnTimeStamp> commit_ts_{UNCOMMIT_TS};

    bool deleted_{false};

    EntryType entry_type_{EntryType::kDummy};
};

struct EntryResult {
    BaseEntry* entry_;
    UniquePtr<String> err_;
};

}

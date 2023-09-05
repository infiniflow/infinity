//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/txn/constants.h"

namespace infinity {

enum EntryType : i8 {
    kDummy,
    kDatabase,
    kTable,
    kView,
    kColumn,
    kSegment,
    kColumnData,
    kBlock,
};

struct BaseEntry {
    explicit
    BaseEntry(EntryType entry_type) : entry_type_(entry_type) {}

    virtual
    ~BaseEntry() = default;

    static inline void
    Commit(BaseEntry* base_entry, TxnTimeStamp commit_ts) {
        base_entry->commit_ts_.store(commit_ts);
    }

    static inline bool
    Committed(BaseEntry* base_entry) {
        return base_entry->commit_ts_ != UNCOMMIT_TS;
    }

public:
    // Reserved
    inline void
    Commit(TxnTimeStamp commit_ts) {
        commit_ts_.store(commit_ts);
    }

    [[nodiscard]] inline bool
    Committed() const {
        return commit_ts_ != UNCOMMIT_TS;
    }

public:
    std::atomic_uint64_t txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    std::atomic<TxnTimeStamp> commit_ts_{UNCOMMIT_TS};

    bool deleted_{false};

    EntryType entry_type_{EntryType::kDummy};
};

struct EntryResult {
    BaseEntry* entry_;
    UniquePtr<String> err_;

    bool Success() {
        return err_ == nullptr;
    }

    bool Fail() {
        return err_ != nullptr;
    }

    String ToString() {
        if(err_ == nullptr) {
            return "Success";
        }
        return *err_.get();
    }
};

}

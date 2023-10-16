//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "common/default_values.h"
#include "common/types/alias/concurrency.h"
#include "common/types/alias/smart_ptr.h"
// #include "storage/txn/txn_state.h"

namespace infinity {

enum EntryType : i8 {
    kDummy,
    kDatabase,
    kTable,
    kIndexDef,
    kIndex,
    kView,
    kColumn,
    kSegment,
    kSegmentColumn,
    kBlock,
    kBlockColumn,
    kDeleteFile,
};

struct BaseEntry {
    explicit BaseEntry(EntryType entry_type) : entry_type_(entry_type) {
        if (entry_type == EntryType::kDummy) {
            commit_ts_ = 0;
            deleted_ = true;
        }
    }

    virtual ~BaseEntry() = default;

    static inline void Commit(BaseEntry *base_entry, TxnTimeStamp commit_ts) { base_entry->commit_ts_.store(commit_ts); }

    static inline bool Committed(BaseEntry *base_entry) { return base_entry->commit_ts_ != UNCOMMIT_TS; }

public:
    // Reserved
    inline void Commit(TxnTimeStamp commit_ts) { commit_ts_.store(commit_ts); }

    [[nodiscard]] inline bool Committed() const { return commit_ts_ != UNCOMMIT_TS; }

public:
    std::atomic_uint64_t txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    std::atomic<TxnTimeStamp> commit_ts_{UNCOMMIT_TS};

    bool deleted_{false};

    EntryType entry_type_{EntryType::kDummy};
};

struct EntryResult {
    BaseEntry *entry_;
    UniquePtr<String> err_;

    bool Success() { return err_ == nullptr; }

    bool Fail() { return err_ != nullptr; }

    String ToString() {
        if (err_ == nullptr) {
            return "Success";
        }
        return *err_.get();
    }
};

} // namespace infinity

//
// Created by jinhai on 23-6-4.
//

module;

import stl;
import default_values;

export module base_entry;

namespace infinity {

export enum EntryType : i8 {
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

export struct BaseEntry {
    explicit BaseEntry(EntryType entry_type) : entry_type_(entry_type) {}

    virtual ~BaseEntry() = default;
    virtual void MergeFrom(BaseEntry &other) {}

    static inline void Commit(BaseEntry *base_entry, TxnTimeStamp commit_ts) { base_entry->commit_ts_.store(commit_ts); }

    static inline bool Committed(BaseEntry *base_entry) { return base_entry->commit_ts_ != UNCOMMIT_TS; }

public:
    // Reserved
    inline void Commit(TxnTimeStamp commit_ts) { commit_ts_.store(commit_ts); }

    [[nodiscard]] inline bool Committed() const { return commit_ts_ != UNCOMMIT_TS; }

public:
    au64 txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    au64 commit_ts_{UNCOMMIT_TS};

    bool deleted_{false};

    EntryType entry_type_{EntryType::kDummy};
};

export struct EntryResult {
    BaseEntry *entry_;
    UniquePtr<String> err_;

    bool Success() { return err_.get() == nullptr; }

    bool Fail() { return err_.get() != nullptr; }

    String ToString() {
        if (err_.get() == nullptr) {
            return "Success";
        }
        return *err_.get();
    }
};

// Merge two reverse-ordered list inplace.
export void MergeLists(List<UniquePtr<BaseEntry>> &list1, List<UniquePtr<BaseEntry>> &list2);

} // namespace infinity

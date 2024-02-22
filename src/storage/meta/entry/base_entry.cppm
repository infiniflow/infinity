// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include <concepts>

export module base_entry;

import stl;
import default_values;

namespace infinity {

class NewCatalog;

export enum class EntryType : i8 {
    kDummy,
    kCatalog,
    kDatabase,
    kTable,
    kTableIndex,
    kColumnIndex,
    kIRSIndex,
    kSegmentColumnIndex,
    kView,
    kColumn,
    kSegment,
    kBlock,
    kBlockColumn,
};

// TODO: remove inheritance
export struct BaseEntry {
    explicit BaseEntry(EntryType entry_type) : entry_type_(entry_type) {
        if (entry_type == EntryType::kDummy) {
            commit_ts_ = 0;
            deleted_ = true;
        }
    }

    virtual ~BaseEntry() = default;
    virtual void MergeFrom(BaseEntry &) {}

    static inline void Commit(BaseEntry *base_entry, TxnTimeStamp commit_ts) { base_entry->commit_ts_.store(commit_ts); }

    static inline bool Committed(BaseEntry *base_entry) { return base_entry->commit_ts_ != UNCOMMIT_TS; }

public:
    // Reserved
    inline void Commit(TxnTimeStamp commit_ts) { commit_ts_.store(commit_ts); }

    [[nodiscard]] inline bool Committed() const { return commit_ts_ != UNCOMMIT_TS; }

    bool Cleanupable(TxnTimeStamp visible_ts) const { return deleted_ && commit_ts_ <= visible_ts; }

public:
    atomic_u64 txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    atomic_u64 commit_ts_{UNCOMMIT_TS};
    bool deleted_{false};

    EntryType entry_type_{EntryType::kDummy};
};

// Merge two reverse-ordered list inplace.
export void MergeLists(List<SharedPtr<BaseEntry>> &list1, List<SharedPtr<BaseEntry>> &list2);

export class CleanupScanner;

export class MetaInterface {
public:
    virtual ~MetaInterface() = default;

    virtual bool PickCleanup(CleanupScanner *scanner) = 0;

    virtual void Cleanup() = 0;
};

export template <typename Meta>
concept MetaConcept = std::derived_from<Meta, MetaInterface>;

export class EntryInterface {
public:
    virtual ~EntryInterface() = default;

    virtual void Cleanup() = 0;

    virtual bool PickCleanup(CleanupScanner *scanner) = 0;
};

export template <typename Entry>
concept EntryConcept = std::derived_from<Entry, EntryInterface>;

export class CleanupScanner {
public:
    explicit CleanupScanner(NewCatalog *catalog, TxnTimeStamp visible_ts) : catalog_(catalog), visible_ts_(visible_ts) {}

    void Scan(TxnTimeStamp visible_ts);

    void AddMeta(UniquePtr<MetaInterface> meta);

    void AddEntry(SharedPtr<EntryInterface> entry);

    TxnTimeStamp visible_ts() const { return visible_ts_; }

private:
    NewCatalog *const catalog_;
    const TxnTimeStamp visible_ts_;

    Vector<SharedPtr<EntryInterface>> entries_;

    Vector<UniquePtr<MetaInterface>> metas_;
};

} // namespace infinity

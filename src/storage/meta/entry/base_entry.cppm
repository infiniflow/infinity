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

export module catalog:base_entry;

import stl;
import default_values;

namespace infinity {

export enum EntryType : i8 {
    kDummy,
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

public:
    atomic_u64 txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    atomic_u64 commit_ts_{UNCOMMIT_TS};
    bool deleted_{false};

    EntryType entry_type_{EntryType::kDummy};
};

// Merge two reverse-ordered list inplace.
export void MergeLists(List<UniquePtr<BaseEntry>> &list1, List<UniquePtr<BaseEntry>> &list2);

} // namespace infinity

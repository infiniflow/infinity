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

#include <cassert>

export module base_entry;

import stl;
import default_values;

namespace infinity {

class Catalog;

export enum class EntryType : i8 {
    kDatabase,
    kTable,
    kTableIndex,
    kSegmentIndex,
    kChunkIndex,
    kView,
    kColumn,
    kSegment,
    kBlock,
    kBlockColumn,
};

export struct BaseEntry {
    explicit BaseEntry(EntryType entry_type, bool is_delete, String encode)
        : deleted_(is_delete), entry_type_(entry_type), encode_(MakeUnique<String>(std::move(encode))) {}

    virtual ~BaseEntry() = default;

    static inline void Commit(BaseEntry *base_entry, TxnTimeStamp commit_ts) { base_entry->commit_ts_.store(commit_ts); }

    static inline bool Committed(BaseEntry *base_entry) { return base_entry->commit_ts_ != UNCOMMIT_TS; }

public:
    // Reserved
    inline void Commit(TxnTimeStamp commit_ts) {
        assert(!Committed() || commit_ts_ == commit_ts);
        assert(commit_ts != UNCOMMIT_TS);
        commit_ts_.store(commit_ts);
    }

    [[nodiscard]] inline bool Committed() const { return commit_ts_ != UNCOMMIT_TS; }

    bool Deleted() const { return deleted_; }

    const String &encode() const { return *encode_; }

    SharedPtr<String> encode_ptr() const { return encode_; }

public:
    atomic_u64 txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    atomic_u64 commit_ts_{UNCOMMIT_TS};
    const bool deleted_;

    const EntryType entry_type_;

private:
    SharedPtr<String> encode_;
};

} // namespace infinity

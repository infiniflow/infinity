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
#include <concepts>

export module base_entry;

import stl;
import default_values;
import infinity_exception;
import third_party;
import logger;
import cleanup_scanner;
import global_resource_usage;
import txn;

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

export String ToString(EntryType entry_type) {
    switch (entry_type) {
        case EntryType::kDatabase: {
            return "database";
        }
        case EntryType::kTable: {
            return "table";
        }
        case EntryType::kTableIndex: {
            return "table_index";
        }
        case EntryType::kSegmentIndex: {
            return "segment_index";
        }
        case EntryType::kChunkIndex: {
            return "chunk_index";
        }
        case EntryType::kView: {
            return "view";
        }
        case EntryType::kColumn: {
            return "column";
        }
        case EntryType::kSegment: {
            return "segment";
        }
        case EntryType::kBlock: {
            return "block";
        }
        case EntryType::kBlockColumn: {
            return "block_column";
        }
    }
}

export struct BaseEntry {
    explicit BaseEntry(EntryType entry_type, bool is_delete, String encode)
        : deleted_(is_delete), entry_type_(entry_type), encode_(MakeUnique<String>(std::move(encode))) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("BaseEntry");
#endif
    }

    BaseEntry(const BaseEntry &other);

    virtual ~BaseEntry() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("BaseEntry");
#endif
    }

    static inline void Commit(BaseEntry *base_entry, TxnTimeStamp commit_ts) { base_entry->commit_ts_.store(commit_ts); }

    static inline bool Committed(BaseEntry *base_entry) { return base_entry->commit_ts_ != UNCOMMIT_TS; }

    virtual void Cleanup(CleanupInfoTracer *info_tracer, bool dropped) = 0;

    virtual void PickCleanup(CleanupScanner *scanner) {};

    virtual Vector<String> GetFilePath(Txn* txn) const = 0;

public:
    // Reserved
    inline void Commit(TxnTimeStamp commit_ts) {
        if (commit_ts_ == UNCOMMIT_TS) {
            commit_ts_.store(commit_ts);
        }
    }

    [[nodiscard]] inline bool Committed() const { return commit_ts_ != UNCOMMIT_TS; }

    bool Deleted() const { return deleted_; }

    const String &encode() const { return *encode_; }

    SharedPtr<String> encode_ptr() const { return encode_; }

    // return if this entry is visible to the `txn`
    virtual bool CheckVisible(Txn *txn) const;

public:
    TransactionID txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    atomic_u64 commit_ts_{UNCOMMIT_TS};
    bool deleted_;

    const EntryType entry_type_;

private:
    SharedPtr<String> encode_;
};

export class BaseMeta {
public:
    BaseMeta() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("BaseMeta");
#endif
    }
    virtual ~BaseMeta() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("BaseMeta");
#endif
    }

    virtual bool PickCleanup(CleanupScanner *scanner) = 0;

    virtual void Cleanup(CleanupInfoTracer *info_tracer = nullptr, bool dropped = true) = 0;

    virtual bool Empty() = 0;
};

export template <typename Meta>
concept MetaConcept = std::derived_from<Meta, BaseMeta>;

export template <typename Entry>
concept EntryConcept = std::derived_from<Entry, BaseEntry>;

} // namespace infinity

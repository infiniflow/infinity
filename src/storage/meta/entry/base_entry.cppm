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
import txn;
import txn_manager;
import infinity_exception;
import third_party;
import txn_state;
import logger;

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
    switch(entry_type) {
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
        : deleted_(is_delete), entry_type_(entry_type), base_dir_(nullptr), encode_(MakeUnique<String>(std::move(encode))) {}

    explicit BaseEntry(EntryType entry_type, bool is_delete, SharedPtr<String> base_dir, String encode)
        : deleted_(is_delete), entry_type_(entry_type), base_dir_(base_dir),
          encode_(MakeUnique<String>(std::move(encode))) {}

    virtual ~BaseEntry() = default;

    static inline void Commit(BaseEntry *base_entry, TxnTimeStamp commit_ts) { base_entry->commit_ts_.store(commit_ts); }

    static inline bool Committed(BaseEntry *base_entry) { return base_entry->commit_ts_ != UNCOMMIT_TS; }

public:
    // Reserved
    inline void Commit(TxnTimeStamp commit_ts) {
        if (commit_ts_ == UNCOMMIT_TS) {
            commit_ts_.store(commit_ts);
        }
    }

    [[nodiscard]] inline bool Committed() const { return commit_ts_ != UNCOMMIT_TS; }

    bool Deleted() const { return deleted_; }

    const String &base_dir() const { return *base_dir_; }

    SharedPtr<String> base_dir_ptr() const { return base_dir_; }

    const String &encode() const { return *encode_; }

    SharedPtr<String> encode_ptr() const { return encode_; }

    // return if this entry is visible to the `txn`
    virtual bool CheckVisible(Txn *txn) const {
        TxnTimeStamp begin_ts = txn->BeginTS();
        if (begin_ts >= commit_ts_ || txn_id_ == txn->TxnID()) {
            return true;
        }
        TxnManager *txn_mgr = txn->txn_mgr();
        if (txn_mgr == nullptr) { // when replay
            String error_message = fmt::format("Replay should not reach here. begin_ts: {}, commit_ts_: {} txn_id: {}, txn_id_: {}",
                                               begin_ts,
                                               commit_ts_,
                                               txn->TxnID(),
                                               txn_id_);
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
        // Check if the entry is in committing process, because commit_ts of the base_entry is set in the Txn::CommitBottom
        return txn_mgr->CheckIfCommitting(txn_id_, begin_ts);
    }

public:
    TransactionID txn_id_{0};
    TxnTimeStamp begin_ts_{0};
    atomic_u64 commit_ts_{UNCOMMIT_TS};
    const bool deleted_;

    const EntryType entry_type_;

    SharedPtr<String> base_dir_;

private:
    SharedPtr<String> encode_;
};

} // namespace infinity

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

#include <algorithm>

import stl;
import base_entry;
import infinity_exception;
import default_values;
import txn_manager;
import txn_state;
import logger;
import parser;
import third_party;

module base_meta;

namespace infinity {
BaseMeta::EntryStatus BaseMeta::AddEntryInternal(u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) const {
    Assert<StorageException>(!entry_list_.empty(), "entry list should at least has a dummy head.");
    // BaseEntry *header_base_entry = entry_list_.front().get();

    auto iter = entry_list_.begin();
    while (true) {
        BaseEntry *header_base_entry = iter->get();
        TxnState header_state = TxnState::kInvalid;
        if (header_base_entry->commit_ts_ < UNCOMMIT_TS) {
            header_state = TxnState::kCommitted;
        } else {
            header_state = txn_mgr->GetTxnState(header_base_entry->txn_id_);
        }
        switch (header_state) {
            case TxnState::kCommitted: {
                if (begin_ts > header_base_entry->commit_ts_) {
                    // begin_ts can see the commit
                    if (header_base_entry->deleted_) {
                        return EntryStatus::kNotExisted; // No conflict. The dummy entry commit_ts is 0 and deleted so can always reach here.
                    } else {
                        return EntryStatus::kExisted;
                    }
                } else {
                    return EntryStatus::kConflict;
                }
            }
            case TxnState::kStarted: {
                if (header_base_entry->txn_id_ == txn_id) {
                    // Same txn
                    if (header_base_entry->deleted_) {
                        return EntryStatus::kNotExisted;
                    } else {
                        return EntryStatus::kExisted;
                    }
                } else {
                    return EntryStatus::kConflict;
                }
            }
            case TxnState::kCommitting: {
                return EntryStatus::kConflict;
            }
            case TxnState::kRollbacking: {
                // current entry is in rollbacking state, continue check previous entry
                ++iter;
                break;
            }
            default: {
                throw StorageException("Invalid entry txn state.");
            }
        }
    }
}

void BaseMeta::DeleteNewEntry(BaseMeta *meta, u64 txn_id, TxnManager *txn_mgr) {
    UniqueLock<RWMutex> w_locker(meta->rw_locker_);
    if (meta->entry_list_.empty()) {
        LOG_TRACE("Attempt to delete not existed entry.");
        return;
    }

    // `std::remove_if` move all elements that satisfy the predicate and move all the last element to the front of list. return value is the end of
    // the moved elements.
    auto removed_iter =
        std::remove_if(meta->entry_list_.begin(), meta->entry_list_.end(), [&](UniquePtr<BaseEntry> &entry) { return entry->txn_id_ == txn_id; });
    // erase the all "moved" elements in the end of list
    meta->entry_list_.erase(removed_iter, meta->entry_list_.end());
}

EntryResult BaseMeta::GetEntry(BaseMeta *meta, u64 txn_id, TxnTimeStamp begin_ts) {
    SharedLock<RWMutex> r_locker(meta->rw_locker_);
    for (const auto &entry : meta->entry_list_) {
        if (entry->entry_type_ == EntryType::kDummy) {
            LOG_TRACE("No valid entry");
            return {.entry_ = nullptr, .err_ = MakeUnique<String>("No valid entry")};
        }

        if (entry->commit_ts_ < UNCOMMIT_TS) {
            // committed
            if (begin_ts > entry->commit_ts_) {
                if (entry->deleted_) {
                    LOG_TRACE("No valid entry");
                    return {.entry_ = nullptr, .err_ = MakeUnique<String>("No valid entry")};
                } else {
                    return {.entry_ = entry.get(), .err_ = nullptr};
                }
            }
        } else if (txn_id == entry->txn_id_) {
            // same txn
            return {.entry_ = entry.get(), .err_ = nullptr};
        }
    }
    LOG_TRACE("No valid entry");
    return {.entry_ = nullptr, .err_ = MakeUnique<String>("No valid entry")};
}
} // namespace infinity
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
#include <vector>

import base_entry;
import stl;
import parser;
import index_def;
import txn_manager;
import default_values;
import txn_state;
import index_def_entry;
import logger;
import third_party;

import infinity_exception;
import table_collection_entry;

module index_def_meta;

namespace infinity {

IndexDefMeta::IndexDefMeta(SharedPtr<String> index_name, TableCollectionEntry *table_collection_entry)
    : index_name_(Move(index_name)), table_collection_entry_(table_collection_entry) {
    // Insert a dummy entry
    entry_list_.emplace_front(MakeUnique<BaseEntry>(EntryType::kDummy));
}

EntryResult IndexDefMeta::CreateNewEntry(IndexDefMeta *index_def_meta,
                                         SharedPtr<IndexDef> index_def,
                                         ConflictType conflict_type,
                                         u64 txn_id,
                                         TxnTimeStamp begin_ts,
                                         TxnManager *txn_mgr) {
    // write lock guard
    UniqueLock<RWMutex> lock(index_def_meta->rw_locker_);
    Assert<StorageException>(!index_def_meta->entry_list_.empty(), "entry list should never be empty.");

    BaseEntry *header_base_entry = index_def_meta->entry_list_.front().get();
    // FIXME shenyushi: use enum instead of int
    int status = -1; // 0: success, 1: conflict, 2: duplicate
    if (header_base_entry->commit_ts_ < UNCOMMIT_TS) {
        // Header has committed or is dummy.
        if (begin_ts > header_base_entry->commit_ts_) {
            if (header_base_entry->deleted_) {
                status = 0; // No conflict. The dummy entry commit_ts is 0 and deleted so can always reach here.
            } else {
                status = 2;
            }
        } else {
            status = 1;
        }
    } else {
        Assert<StorageException>(header_base_entry->entry_type_ != EntryType::kDummy, "Dummy entry should always be committed.");
        // Uncommitted
        TxnState header_index_def_entry_state = txn_mgr->GetTxnState(header_base_entry->txn_id_);
        switch (header_index_def_entry_state) {
            case TxnState::kStarted: {
                if (header_base_entry->txn_id_ == txn_id) {
                    // Same txn
                    if (header_base_entry->deleted_) {
                        status = 0;
                    } else {
                        status = 2;
                    }
                } else {
                    status = 1;
                }
                break;
            }
            case TxnState::kCommitting: {
                status = 1;
                break;
            }
            case TxnState::kRollbacking: {
                // Remove the header entry
                index_def_meta->entry_list_.pop_front();
                status = 0;
                break;
            }
            default: {
                Error<StorageException>("Invalid index entry txn state.");
            }
        }
    }
    Assert<StorageException>(status != -1, "Invalid status.");
    if (status == 0) {
        auto index_def_entry = MakeUnique<IndexDefEntry>(index_def, index_def_meta, txn_id, begin_ts);
        IndexDefEntry *res = index_def_entry.get();
        index_def_meta->entry_list_.emplace_front(Move(index_def_entry));
        return {.entry_ = res, .err_ = nullptr};
    } else if (status == 1) {
        // Write-Write conflict
        LOG_TRACE("Write-Write conflict: There is a committed index: which is later than current transaction.");
        return {.entry_ = nullptr,
                .err_ = MakeUnique<String>("Write-Write conflict. There is a committed index which is later than current transaction.")};
    } else {
        LOG_TRACE(Format("Duplicated index: {}.", *index_def->index_name_));
        switch (conflict_type) {
            case ConflictType::kIgnore: {
                return {.entry_ = nullptr, .err_ = nullptr};
            }
            case ConflictType::kError: {
                return {.entry_ = nullptr, .err_ = MakeUnique<String>("Duplicated index.")};
            }
            default: {
                Error<StorageException>("Invalid conflict type");
            }
        }
        Error<StorageException>("Cannot reach here");
    }
}

EntryResult
IndexDefMeta::DropNewEntry(IndexDefMeta *index_def_meta, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    IndexDefEntry *res = nullptr;
    UniqueLock<RWMutex> lock(index_def_meta->rw_locker_);

    Assert<StorageException>(!index_def_meta->entry_list_.empty(), "entry list should never be empty.");
    BaseEntry *header_base_entry = index_def_meta->entry_list_.front().get();

    int status = -1; // 0: success, 1: conflict, 2: not exists
    // auto index_def_entry = static_cast<IndexDefEntry *>(entry);
    if (header_base_entry->commit_ts_ < UNCOMMIT_TS) {
        // Committed
        if (begin_ts > header_base_entry->commit_ts_) {
            // No conflict
            if (header_base_entry->deleted_) {
                status = 2;
            } else {
                status = 0;
            }
        } else {
            status = 1;
        }
    } else {
        Assert<StorageException>(header_base_entry->entry_type_ != EntryType::kDummy, "Dummy entry should always be committed.");
        // Uncommitted
        TxnState header_index_def_entry_state = txn_mgr->GetTxnState(header_base_entry->txn_id_);
        switch (header_index_def_entry_state) {
            case TxnState::kStarted: {
                if (header_base_entry->txn_id_ == txn_id) {
                    // Same txn
                    if (header_base_entry->deleted_) {
                        status = 0;
                    } else {
                        status = 2;
                    }
                } else {
                    status = 1;
                }
                break;
            }
            case TxnState::kCommitting: {
                status = 1;
                break;
            }
            case TxnState::kRollbacking: {
                // Remove the header entry
                index_def_meta->entry_list_.pop_front();
                status = 0;
                break;
            }
            default: {
                Error<StorageException>("Invalid index entry txn state.");
            }
        }
    }

    Assert<StorageException>(status != -1, "Invalid status.");
    if (status == 0) {
        auto index_def_entry = MakeUnique<IndexDefEntry>(nullptr, index_def_meta, txn_id, begin_ts);
        res = index_def_entry.get();
        res->deleted_ = true;
        index_def_meta->entry_list_.emplace_front(Move(index_def_entry));
        return {.entry_ = res, .err_ = nullptr};
    } else if (status == 1) {
        // Write-Write conflict
        LOG_TRACE("Write-Write conflict: There is a committed index: which is later than current transaction.");
        return {.entry_ = nullptr,
                .err_ = MakeUnique<String>("Write-Write conflict. There is a committed index which is later than current transaction.")};
    } else {
        LOG_TRACE(Format("Attempt to drop not existed index: {}.", *index_def_meta->index_name_));
        switch (conflict_type) {
            case ConflictType::kIgnore: {
                return {.entry_ = nullptr, .err_ = nullptr};
            }
            case ConflictType::kError: {
                return {.entry_ = nullptr, .err_ = MakeUnique<String>("Duplicated index.")};
            }
            default: {
                Error<StorageException>("Invalid conflict type");
            }
        }
        Error<StorageException>("Cannot reach here");
    }
}

void IndexDefMeta::DeleteNewEntry(IndexDefMeta *index_def_meta, u64 txn_id, TxnManager *txn_mgr) {
    UniqueLock<RWMutex> rw_locker(index_def_meta->rw_locker_);
    if (index_def_meta->entry_list_.empty()) {
        LOG_TRACE("Attempt to delete not existed index entry.");
        return;
    }

    // `std::remove_if` if move all elements that satisfy the predicate to the first part of the container and return the iterator to the end of first
    // part
    auto removed_iter = std::remove_if(index_def_meta->entry_list_.begin(), index_def_meta->entry_list_.end(), [&](UniquePtr<BaseEntry> &entry) {
        return entry->txn_id_ == txn_id;
    });
    // erase the all "moved" elements of the second part
    index_def_meta->entry_list_.erase(removed_iter, index_def_meta->entry_list_.end());
}

EntryResult IndexDefMeta::GetEntry(IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts) {
    SharedLock<RWMutex> r_locker(index_def_meta->rw_locker_);
    for (const auto &index_def_entry : index_def_meta->entry_list_) {
        if (index_def_entry->entry_type_ == EntryType::kDummy) {
            LOG_TRACE("No valid index def entry");
            return {.entry_ = nullptr, .err_ = MakeUnique<String>("No valid index def entry")};
        }

        if (index_def_entry->commit_ts_ < UNCOMMIT_TS) {
            // committed
            if (begin_ts > index_def_entry->commit_ts_) {
                if (index_def_entry->deleted_) {
                    LOG_TRACE("No valid index def entry");
                    return {.entry_ = nullptr, .err_ = MakeUnique<String>("No valid index def entry")};
                } else {
                    return {.entry_ = index_def_entry.get(), .err_ = nullptr};
                }
            }
        } else if (txn_id == index_def_entry->txn_id_) {
            // same txn
            return {.entry_ = index_def_entry.get(), .err_ = nullptr};
        }
    }
    LOG_TRACE("No valid index def entry");
    return {.entry_ = nullptr, .err_ = MakeUnique<String>("No valid index def entry")};
}

SharedPtr<String> IndexDefMeta::ToString(IndexDefMeta *index_def_meta) {
    Error<StorageException>("Not implemented");
    return nullptr;
}

Json IndexDefMeta::Serialize(const IndexDefMeta *index_def_meta) {
    Json json;

    json["index_name"] = *index_def_meta->index_name_;
    for (const auto &entry : index_def_meta->entry_list_) {
        if (entry->entry_type_ == EntryType::kIndexDef) {
            json["entries"].emplace_back(IndexDefEntry::Serialize(static_cast<IndexDefEntry *>(entry.get())));
        } else if (entry->entry_type_ == EntryType::kDummy) {
            LOG_TRACE(Format("Skip dummy entry during serialize index {} meta", *index_def_meta->index_name_));
        } else {
            Error<StorageException>("Unexpected entry type.");
        }
    }
    return json;
}

UniquePtr<IndexDefMeta> IndexDefMeta::Deserialize(const Json &index_def_meta_json, TableCollectionEntry *table_collection_entry) {
    auto index_name = MakeShared<String>(index_def_meta_json["index_name"]);
    LOG_TRACE(Format("load index {}", *index_name));

    auto res = MakeUnique<IndexDefMeta>(Move(index_name), table_collection_entry);
    if (index_def_meta_json.contains("entries")) {
        auto &entries = index_def_meta_json["entries"];
        for (auto iter = entries.rbegin(); iter != entries.rend(); iter++) {
            auto entry = IndexDefEntry::Deserialize(*iter, res.get());
            res->entry_list_.emplace_front(Move(entry));
        }
    }
    return res;
}
} // namespace infinity

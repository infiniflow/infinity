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
import logger;
import third_party;

import infinity_exception;
import table_collection_entry;
import status;
import iresearch_datastore;
import table_index_entry;
import base_meta;

module table_index_meta;

namespace infinity {

struct SegmentEntry;

TableIndexMeta::TableIndexMeta(TableCollectionEntry *table_collection_entry, SharedPtr<String> index_name)
    : index_name_(Move(index_name)), table_collection_entry_(table_collection_entry) {}

Status TableIndexMeta::CreateTableIndexEntry(TableIndexMeta *table_index_meta,
                                             const SharedPtr<IndexDef> &index_def,
                                             ConflictType conflict_type,
                                             u64 txn_id,
                                             TxnTimeStamp begin_ts,
                                             TxnManager *txn_mgr,
                                             BaseEntry *&new_index_entry) {
    return BaseMeta::CheckDDLResult(CreateTableIndexEntryInternal(table_index_meta, index_def, txn_id, begin_ts, txn_mgr, new_index_entry),
                                    conflict_type);
}

Status TableIndexMeta::CreateTableIndexEntryInternal(TableIndexMeta *table_index_meta,
                                                     const SharedPtr<IndexDef> &index_def,
                                                     u64 txn_id,
                                                     TxnTimeStamp begin_ts,
                                                     TxnManager *txn_mgr,
                                                     BaseEntry *&new_index_entry) {
    UniqueLock<RWMutex> rw_locker(table_index_meta->rw_locker_);

    if (table_index_meta->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        table_index_meta->entry_list_.emplace_back(Move(dummy_entry));

        // Create a new table index entry
        auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, table_index_meta, txn_id, begin_ts);
        new_index_entry = table_index_entry.get();
        table_index_meta->entry_list_.emplace_front(Move(table_index_entry));
        LOG_TRACE("New table index entry is added.");
        return Status::OK();
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry *header_base_entry = table_index_meta->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, table_index_meta, txn_id, begin_ts);
            new_index_entry = table_index_entry.get();
            table_index_meta->entry_list_.emplace_front(Move(table_index_entry));
            LOG_TRACE("New table index entry is added.");
            return Status::OK();
        }

        TableIndexEntry *header_entry = (TableIndexEntry *)header_base_entry;
        if (header_entry->Committed()) {
            if (begin_ts > header_entry->commit_ts_) {
                if (header_entry->deleted_) {
                    // No conflict
                    auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, table_index_meta, txn_id, begin_ts);
                    new_index_entry = table_index_entry.get();
                    table_index_meta->entry_list_.emplace_front(Move(table_index_entry));
                    LOG_TRACE("New table index entry is added.");
                    return Status::OK();
                } else {
                    // Duplicated index name
                    UniquePtr<String> err_msg = MakeUnique<String>(Format("Duplicated index name: {}.", *table_index_meta->index_name_));
                    LOG_ERROR(*err_msg);
                    return Status(ErrorCode::kDuplicate, Move(err_msg));
                }
            } else {
                // Write-Write conflict
                UniquePtr<String> err_msg =
                    MakeUnique<String>(Format("Write-write conflict: There is a committed database which is later than current transaction."));
                LOG_ERROR(*err_msg);
                return Status(ErrorCode::kWWConflict, Move(err_msg));
            }
        } else {

            TxnState head_db_entry_state = txn_mgr->GetTxnState(header_entry->txn_id_);

            switch (head_db_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if (header_entry->txn_id_ == txn_id) {
                        // Same txn
                        if (header_entry->deleted_) {
                            // No conflict
                            auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, table_index_meta, txn_id, begin_ts);
                            new_index_entry = table_index_entry.get();
                            table_index_meta->entry_list_.emplace_front(Move(table_index_entry));
                            LOG_TRACE("New table index entry is added.");
                            return Status::OK();
                        } else {
                            UniquePtr<String> err_msg = MakeUnique<String>(Format("Duplicated index name: {}.", *table_index_meta->index_name_));
                            LOG_ERROR(*err_msg);
                            return Status(ErrorCode::kDuplicate, Move(err_msg));
                        }
                    } else {
                        UniquePtr<String> err_msg = MakeUnique<String>(Format("Write-write conflict: There is a uncommitted transaction."));
                        LOG_ERROR(*err_msg);
                        return Status(ErrorCode::kWWConflict, Move(err_msg));
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    UniquePtr<String> err_msg = MakeUnique<String>(
                        Format("Write-write conflict: There is a committing/committed database which is later than current transaction."));
                    LOG_ERROR(*err_msg);
                    return Status(ErrorCode::kWWConflict, Move(err_msg));
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    table_index_meta->entry_list_.erase(table_index_meta->entry_list_.begin());

                    // Append new one
                    auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, table_index_meta, txn_id, begin_ts);
                    new_index_entry = table_index_entry.get();
                    table_index_meta->entry_list_.emplace_front(Move(table_index_entry));
                    LOG_TRACE("New table index entry is added.");
                    return Status::OK();
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Invalid db entry txn state");
                    LOG_ERROR(*err_msg);
                    return Status(ErrorCode::kUndefined, Move(err_msg));
                }
            }
        }
    }
}

Status TableIndexMeta::DropTableIndexEntry(TableIndexMeta *table_index_meta,
                                           ConflictType conflict_type,
                                           u64 txn_id,
                                           TxnTimeStamp begin_ts,
                                           TxnManager *txn_mgr,
                                           BaseEntry *&dropped_index_entry) {
    return BaseMeta::CheckDDLResult(DropTableIndexEntryInternal(table_index_meta, txn_id, begin_ts, txn_mgr, dropped_index_entry), conflict_type);
}

Status TableIndexMeta::DropTableIndexEntryInternal(TableIndexMeta *table_index_meta,
                                                   u64 txn_id,
                                                   TxnTimeStamp begin_ts,
                                                   TxnManager *,
                                                   BaseEntry *&dropped_index_entry) {
    UniqueLock<RWMutex> w_locker(table_index_meta->rw_locker_);

    if (table_index_meta->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty index entry list.");
        LOG_ERROR(*err_msg);
        return Status(ErrorCode::kNotFound, Move(err_msg));
    }

    BaseEntry *header_base_entry = table_index_meta->entry_list_.front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        UniquePtr<String> err_msg = MakeUnique<String>("No valid index entry.");
        LOG_ERROR(*err_msg);
        return Status(ErrorCode::kNotFound, Move(err_msg));
    }

    TableIndexEntry *header_index_entry = (TableIndexEntry *)header_base_entry;
    if (header_index_entry->Committed()) {
        if (begin_ts > header_index_entry->commit_ts_) {
            // No conflict
            if (header_index_entry->deleted_) {
                UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped before.");
                LOG_TRACE(*err_msg);
                return Status(ErrorCode::kNotFound, Move(err_msg));
            }

            // Append new one to drop index
            auto table_index_entry = TableIndexEntry::NewDropTableIndexEntry(table_index_meta, txn_id, begin_ts);
            dropped_index_entry = table_index_entry.get();
            dropped_index_entry->deleted_ = true;
            table_index_meta->entry_list_.emplace_front(Move(table_index_entry));
            return Status::OK();
        } else {
            // Write-Write conflict
            UniquePtr<String> err_msg =
                MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.");
            LOG_ERROR(*err_msg);
            return Status(ErrorCode::kWWConflict, Move(err_msg));
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_index_entry->txn_id_) {
            // Same txn, remove the header db entry
            dropped_index_entry = header_index_entry;
            table_index_meta->entry_list_.erase(table_index_meta->entry_list_.begin());
            return Status::OK();
        } else {
            // Not same txn, issue WW conflict
            UniquePtr<String> err_msg = MakeUnique<String>("Write-write conflict: There is another uncommitted db entry.");
            LOG_ERROR(*err_msg);
            return Status(ErrorCode::kWWConflict, Move(err_msg));
        }
    }
}

SharedPtr<String> TableIndexMeta::ToString(TableIndexMeta *) { throw StorageException("Not implemented"); }

Json TableIndexMeta::Serialize(const TableIndexMeta *table_index_meta, TxnTimeStamp max_commit_ts) {
    Json json;

    for (const auto &entry : table_index_meta->entry_list_) {
        if (entry->entry_type_ == EntryType::kTableIndex) {
            json["entries"].emplace_back(TableIndexEntry::Serialize(static_cast<TableIndexEntry *>(entry.get()), max_commit_ts));
        } else if (entry->entry_type_ == EntryType::kDummy) {
            continue;
        } else {
            Error<StorageException>("Unexpected entry type.");
        }
    }
    return json;
}

UniquePtr<TableIndexMeta>
TableIndexMeta::Deserialize(const Json &table_index_meta_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr) {
    LOG_TRACE(Format("load index"));

    SharedPtr<String> index_name = MakeShared<String>(table_index_meta_json["index_name"]);
    auto res = MakeUnique<TableIndexMeta>(table_entry, index_name);
    if (table_index_meta_json.contains("entries")) {
        auto &entries = table_index_meta_json["entries"];
        // traverse reversely because a dummy head has been inserted
        for (auto iter = entries.rbegin(); iter != entries.rend(); iter++) {
            auto entry = TableIndexEntry::Deserialize(*iter, res.get(), buffer_mgr, table_entry);
            res->entry_list_.emplace_front(Move(entry));
        }
    }
    return res;
}

Status TableIndexMeta::GetEntry(TableIndexMeta *meta, u64 txn_id, TxnTimeStamp begin_ts, BaseEntry *&base_entry) {
    SharedLock<RWMutex> r_locker(meta->rw_locker_);
    for (const auto &entry : meta->entry_list_) {
        if (entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
            LOG_ERROR(*err_msg);
            return Status(ErrorCode::kNotFound, Move(err_msg));
        }

        if (entry->commit_ts_ < UNCOMMIT_TS) {
            // committed
            if (begin_ts > entry->commit_ts_) {
                if (entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
                    LOG_ERROR(*err_msg);
                    return Status(ErrorCode::kNotFound, Move(err_msg));
                } else {
                    base_entry = entry.get();
                    return Status::OK();
                }
            }
        } else if (txn_id == entry->txn_id_) {
            // same txn
            base_entry = entry.get();
            return Status::OK();
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
    LOG_ERROR(*err_msg);
    return Status(ErrorCode::kNotFound, Move(err_msg));
}

void TableIndexMeta::DeleteNewEntry(TableIndexMeta *meta, u64 txn_id, TxnManager *) {
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

} // namespace infinity

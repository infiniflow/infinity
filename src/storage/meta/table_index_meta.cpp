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

module catalog;

import stl;
import parser;
import index_def;
import txn_manager;
import default_values;
import txn_state;
import logger;
import third_party;

import infinity_exception;
import status;
import iresearch_datastore;

namespace infinity {

struct SegmentEntry;

TableIndexMeta::TableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name) : index_name_(Move(index_name)), table_entry_(table_entry) {}

Tuple<TableIndexEntry *, Status> TableIndexMeta::CreateTableIndexEntry(const SharedPtr<IndexDef> &index_def,
                                                                       ConflictType conflict_type,
                                                                       u64 txn_id,
                                                                       TxnTimeStamp begin_ts,
                                                                       TxnManager *txn_mgr) {

    auto [table_index_entry, status] = CreateTableIndexEntryInternal(index_def, txn_id, begin_ts, txn_mgr);
    switch (conflict_type) {
        case ConflictType::kError: {
            return {table_index_entry, status};
        }
        case ConflictType::kIgnore: {
            if (status.code() == ErrorCode::kDuplicate or status.code() == ErrorCode::kNotFound) {
                return {table_index_entry, Status::OK()};
            } else {
                return {table_index_entry, status};
            }
        }
        default: {
            Error<StorageException>("Invalid conflict type.");
            return {table_index_entry, status};
        }
    }
}

Tuple<TableIndexEntry *, Status>
TableIndexMeta::CreateTableIndexEntryInternal(const SharedPtr<IndexDef> &index_def, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    TableIndexEntry *table_index_entry_ptr{nullptr};

    UniqueLock<RWMutex> rw_locker(this->rw_locker_);

    if (this->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        this->entry_list_.emplace_back(Move(dummy_entry));

        // Create a new table index entry
        auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, this, txn_id, begin_ts);
        table_index_entry_ptr = table_index_entry.get();
        this->entry_list_.emplace_front(Move(table_index_entry));
        LOG_TRACE("New table index entry is added.");
        return {table_index_entry_ptr, Status::OK()};
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry *header_base_entry = this->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, this, txn_id, begin_ts);
            table_index_entry_ptr = table_index_entry.get();
            this->entry_list_.emplace_front(Move(table_index_entry));
            LOG_TRACE("New table index entry is added.");
            return {table_index_entry_ptr, Status::OK()};
        }

        TableIndexEntry *header_entry = (TableIndexEntry *)header_base_entry;
        if (header_entry->Committed()) {
            if (begin_ts > header_entry->commit_ts_) {
                if (header_entry->deleted_) {
                    // No conflict
                    auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, this, txn_id, begin_ts);
                    table_index_entry_ptr = table_index_entry.get();
                    this->entry_list_.emplace_front(Move(table_index_entry));
                    LOG_TRACE("New table index entry is added.");
                    return {table_index_entry_ptr, Status::OK()};
                } else {
                    // Duplicated index name
                    UniquePtr<String> err_msg = MakeUnique<String>(Format("Duplicated index name: {}.", *this->index_name_));
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kDuplicate, Move(err_msg))};
                }
            } else {
                // Write-Write conflict
                UniquePtr<String> err_msg =
                    MakeUnique<String>(Format("Write-write conflict: There is a committed database which is later than current transaction."));
                LOG_ERROR(*err_msg);
                return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
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
                            auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, this, txn_id, begin_ts);
                            table_index_entry_ptr = table_index_entry.get();
                            this->entry_list_.emplace_front(Move(table_index_entry));
                            LOG_TRACE("New table index entry is added.");
                            return {table_index_entry_ptr, Status::OK()};
                        } else {
                            UniquePtr<String> err_msg = MakeUnique<String>(Format("Duplicated index name: {}.", *this->index_name_));
                            LOG_ERROR(*err_msg);
                            return {nullptr, Status(ErrorCode::kDuplicate, Move(err_msg))};
                        }
                    } else {
                        UniquePtr<String> err_msg = MakeUnique<String>(Format("Write-write conflict: There is a uncommitted transaction."));
                        LOG_ERROR(*err_msg);
                        return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    UniquePtr<String> err_msg = MakeUnique<String>(
                        Format("Write-write conflict: There is a committing/committed database which is later than current transaction."));
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    this->entry_list_.erase(this->entry_list_.begin());

                    // Append new one
                    auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, this, txn_id, begin_ts);
                    table_index_entry_ptr = table_index_entry.get();
                    this->entry_list_.emplace_front(Move(table_index_entry));
                    LOG_TRACE("New table index entry is added.");
                    return {table_index_entry_ptr, Status::OK()};
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Invalid db entry txn state");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kUndefined, Move(err_msg))};
                }
            }
        }
    }
}

Tuple<TableIndexEntry *, Status>
TableIndexMeta::DropTableIndexEntry(ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {

    auto [table_index_entry, status] = DropTableIndexEntryInternal(txn_id, begin_ts, txn_mgr);
    switch (conflict_type) {
        case ConflictType::kError: {
            return {table_index_entry, status};
        }
        case ConflictType::kIgnore: {
            if (status.code() == ErrorCode::kDuplicate or status.code() == ErrorCode::kNotFound) {
                return {table_index_entry, Status::OK()};
            } else {
                return {table_index_entry, status};
            }
        }
        default: {
            Error<StorageException>("Invalid conflict type.");
            return {table_index_entry, status};
        }
    }
}

Tuple<TableIndexEntry *, Status> TableIndexMeta::DropTableIndexEntryInternal(u64 txn_id, TxnTimeStamp begin_ts, TxnManager *) {

    TableIndexEntry *table_index_entry_ptr{nullptr};
    UniqueLock<RWMutex> w_locker(this->rw_locker_);

    if (this->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty index entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    BaseEntry *header_base_entry = this->entry_list_.front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        UniquePtr<String> err_msg = MakeUnique<String>("No valid index entry.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    TableIndexEntry *header_index_entry = (TableIndexEntry *)header_base_entry;
    if (header_index_entry->Committed()) {
        if (begin_ts > header_index_entry->commit_ts_) {
            // No conflict
            if (header_index_entry->deleted_) {
                UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped before.");
                LOG_TRACE(*err_msg);
                return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
            }

            // Append new one to drop index
            auto table_index_entry = TableIndexEntry::NewDropTableIndexEntry(this, txn_id, begin_ts);
            table_index_entry_ptr = table_index_entry.get();
            table_index_entry_ptr->deleted_ = true;
            this->entry_list_.emplace_front(Move(table_index_entry));
            return {table_index_entry_ptr, Status::OK()};
        } else {
            // Write-Write conflict
            UniquePtr<String> err_msg =
                MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_index_entry->txn_id_) {
            // Same txn, remove the header db entry
            table_index_entry_ptr = header_index_entry;
            this->entry_list_.erase(this->entry_list_.begin());
            return {table_index_entry_ptr, Status::OK()};
        } else {
            // Not same txn, issue WW conflict
            UniquePtr<String> err_msg = MakeUnique<String>("Write-write conflict: There is another uncommitted db entry.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
        }
    }
}

SharedPtr<String> TableIndexMeta::ToString(TableIndexMeta *) { throw StorageException("Not implemented"); }

Json TableIndexMeta::Serialize(TableIndexMeta *table_index_meta, TxnTimeStamp max_commit_ts) {
    Json json_res;

    Vector<TableIndexEntry *> table_index_entry_candidates;
    {
        SharedLock<RWMutex> lck(table_index_meta->rw_locker_);
        json_res["index_name"] = *table_index_meta->index_name_;

        table_index_entry_candidates.reserve(table_index_meta->entry_list_.size());
        for (const auto &base_entry : table_index_meta->entry_list_) {
            if (base_entry->entry_type_ == EntryType::kDummy) {
                continue;
            }
            if (base_entry->entry_type_ != EntryType::kTableIndex) {
                Error<StorageException>("Unexpected entry type during serialize table index meta");
            }
            if (base_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                table_index_entry_candidates.push_back((TableIndexEntry *)base_entry.get());
            }
        }
    }

    for (const auto &table_index_entry : table_index_entry_candidates) {
        json_res["index_entries"].emplace_back(TableIndexEntry::Serialize(table_index_entry, max_commit_ts));
    }
    return json_res;
}

UniquePtr<TableIndexMeta> TableIndexMeta::Deserialize(const Json &table_index_meta_json, TableEntry *table_entry, BufferManager *buffer_mgr) {
    LOG_TRACE(Format("load index"));

    SharedPtr<String> index_name = MakeShared<String>(table_index_meta_json["index_name"]);
    auto res = MakeUnique<TableIndexMeta>(table_entry, index_name);
    if (table_index_meta_json.contains("index_entries")) {
        auto &entries = table_index_meta_json["index_entries"];
        // traverse reversely because a dummy head has been inserted
        for (auto iter = entries.rbegin(); iter != entries.rend(); iter++) {
            auto entry = TableIndexEntry::Deserialize(*iter, res.get(), buffer_mgr, table_entry);
            res->entry_list_.emplace_front(Move(entry));
        }
    }
    return res;
}

Tuple<TableIndexEntry *, Status> TableIndexMeta::GetEntry(u64 txn_id, TxnTimeStamp begin_ts) {

    TableIndexEntry *table_index_entry{nullptr};

    SharedLock<RWMutex> r_locker(this->rw_locker_);
    for (const auto &entry : this->entry_list_) {
        if (entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
        }

        if (entry->commit_ts_ < UNCOMMIT_TS) {
            // committed
            if (begin_ts > entry->commit_ts_) {
                if (entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
                } else {
                    table_index_entry = static_cast<TableIndexEntry *>(entry.get());
                    return {table_index_entry, Status::OK()};
                }
            }
        } else if (txn_id == entry->txn_id_) {
            // same txn
            table_index_entry = static_cast<TableIndexEntry *>(entry.get());
            return {table_index_entry, Status::OK()};
        }
    }

    UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
    LOG_ERROR(*err_msg);
    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
}

void TableIndexMeta::DeleteNewEntry(u64 txn_id, TxnManager *) {
    UniqueLock<RWMutex> w_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        LOG_TRACE("Attempt to delete not existed entry.");
        return;
    }

    // `std::remove_if` move all elements that satisfy the predicate and move all the last element to the front of list. return value is the end of
    // the moved elements.
    auto removed_iter =
        std::remove_if(this->entry_list_.begin(), this->entry_list_.end(), [&](UniquePtr<BaseEntry> &entry) { return entry->txn_id_ == txn_id; });
    // erase the all "moved" elements in the end of list
    this->entry_list_.erase(removed_iter, this->entry_list_.end());
}

void TableIndexMeta::MergeFrom(TableIndexMeta &other) {
    if (!IsEqual(*this->index_name_, *other.index_name_)) {
        Error<StorageException>("TableIndexMeta::MergeFrom requires index_name_ match");
    }
    MergeLists(this->entry_list_, other.entry_list_);
}

} // namespace infinity

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

TableIndexMeta::TableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name)
    : index_name_(std::move(index_name)), table_entry_(table_entry) {}

UniquePtr<TableIndexMeta> TableIndexMeta::NewTableIndexMeta(TableEntry *table_entry, SharedPtr<String> index_name) {
    auto table_index_meta = MakeUnique<TableIndexMeta>(table_entry, index_name);
    return table_index_meta;
}

Tuple<TableIndexEntry *, Status> TableIndexMeta::CreateTableIndexEntry(const SharedPtr<IndexDef> &index_def,
                                                                       ConflictType conflict_type,
                                                                       TransactionID txn_id,
                                                                       TxnTimeStamp begin_ts,
                                                                       TxnManager *txn_mgr,
                                                                       bool is_replay,
                                                                       String replay_table_index_dir) {
    auto [table_index_entry, status] = CreateTableIndexEntryInternal(index_def, txn_id, begin_ts, txn_mgr, is_replay, replay_table_index_dir);
    switch (conflict_type) {
        case ConflictType::kError: {
            return {table_index_entry, status};
        }
        case ConflictType::kIgnore: {
            if (status.code() == ErrorCode::kDuplicateIndexName or status.code() == ErrorCode::kIndexNotExist) {
                return {table_index_entry, Status::OK()};
            } else {
                return {table_index_entry, status};
            }
        }
        default: {
            UnrecoverableError("Invalid conflict type.");
            return {table_index_entry, status};
        }
    }
}

Tuple<TableIndexEntry *, Status> TableIndexMeta::CreateTableIndexEntryInternal(const SharedPtr<IndexDef> &index_def,
                                                                               TransactionID txn_id,
                                                                               TxnTimeStamp begin_ts,
                                                                               TxnManager *txn_mgr,
                                                                               bool is_replay,
                                                                               String replay_table_index_dir) {
    TableIndexEntry *table_index_entry_ptr{nullptr};

    Txn *txn{nullptr};
    if (txn_mgr != nullptr) {
        txn = txn_mgr->GetTxn(txn_id);
    }

    std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker_);

    if (this->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        this->entry_list_.emplace_back(std::move(dummy_entry));

        // Create a new table index entry
        auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, this, txn, txn_id, begin_ts, is_replay, replay_table_index_dir);
        table_index_entry_ptr = table_index_entry.get();
        this->entry_list_.emplace_front(std::move(table_index_entry));
        LOG_TRACE("New table index entry is added.");
        return {table_index_entry_ptr, Status::OK()};
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry *header_base_entry = this->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            auto table_index_entry = TableIndexEntry::NewTableIndexEntry(index_def, this, txn, txn_id, begin_ts, is_replay, replay_table_index_dir);
            table_index_entry_ptr = table_index_entry.get();
            this->entry_list_.emplace_front(std::move(table_index_entry));
            LOG_TRACE("New table index entry is added.");
            return {table_index_entry_ptr, Status::OK()};
        }

        TableIndexEntry *header_entry = (TableIndexEntry *)header_base_entry;
        if (header_entry->Committed()) {
            if (begin_ts > header_entry->commit_ts_) {
                if (header_entry->deleted_) {
                    // No conflict
                    auto table_index_entry =
                        TableIndexEntry::NewTableIndexEntry(index_def, this, txn, txn_id, begin_ts, is_replay, replay_table_index_dir);
                    table_index_entry_ptr = table_index_entry.get();
                    this->entry_list_.emplace_front(std::move(table_index_entry));
                    LOG_TRACE("New table index entry is added.");
                    return {table_index_entry_ptr, Status::OK()};
                } else {
                    // Duplicated index name
                    UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Duplicated index name: {}.", *this->index_name_));
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kDuplicateIndexName, std::move(err_msg))};
                }
            } else {
                // Write-Write conflict
                UniquePtr<String> err_msg = MakeUnique<String>(
                    fmt::format("Write-write conflict: There is a committed TableIndexEntry which is later than current transaction."));
                LOG_ERROR(*err_msg);
                return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
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
                            auto table_index_entry =
                                TableIndexEntry::NewTableIndexEntry(index_def, this, txn, txn_id, begin_ts, is_replay, replay_table_index_dir);
                            table_index_entry_ptr = table_index_entry.get();
                            this->entry_list_.emplace_front(std::move(table_index_entry));
                            LOG_TRACE("New table index entry is added.");
                            return {table_index_entry_ptr, Status::OK()};
                        } else {
                            UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Duplicated index name: {}.", *this->index_name_));
                            LOG_ERROR(*err_msg);
                            return {nullptr, Status(ErrorCode::kDuplicateIndexName, std::move(err_msg))};
                        }
                    } else {
                        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Write-write conflict: There is a uncommitted transaction."));
                        LOG_ERROR(*err_msg);
                        return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    UniquePtr<String> err_msg = MakeUnique<String>(
                        fmt::format("Write-write conflict: There is a committing/committed database which is later than current transaction."));
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    this->entry_list_.erase(this->entry_list_.begin());

                    // Append new one
                    auto table_index_entry =
                        TableIndexEntry::NewTableIndexEntry(index_def, this, txn, txn_id, begin_ts, is_replay, replay_table_index_dir);
                    table_index_entry_ptr = table_index_entry.get();
                    this->entry_list_.emplace_front(std::move(table_index_entry));
                    LOG_TRACE("New table index entry is added.");
                    return {table_index_entry_ptr, Status::OK()};
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Invalid db entry txn state");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kUnexpectedError, std::move(err_msg))};
                }
            }
        }
    }
}

Tuple<TableIndexEntry *, Status>
TableIndexMeta::DropTableIndexEntry(ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {

    auto [table_index_entry, status] = DropTableIndexEntryInternal(txn_id, begin_ts, txn_mgr);
    switch (conflict_type) {
        case ConflictType::kError: {
            return {table_index_entry, status};
        }
        case ConflictType::kIgnore: {
            if (status.code() == ErrorCode::kDuplicateIndexName or status.code() == ErrorCode::kIndexNotExist) {
                return {table_index_entry, Status::OK()};
            } else {
                return {table_index_entry, status};
            }
        }
        default: {
            UnrecoverableError("Invalid conflict type.");
            return {table_index_entry, status};
        }
    }
}

Tuple<TableIndexEntry *, Status> TableIndexMeta::DropTableIndexEntryInternal(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *) {

    TableIndexEntry *table_index_entry_ptr{nullptr};
    std::unique_lock<std::shared_mutex> w_locker(this->rw_locker_);

    if (this->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty index entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kIndexNotExist, std::move(err_msg))};
    }

    BaseEntry *header_base_entry = this->entry_list_.front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        UniquePtr<String> err_msg = MakeUnique<String>("No valid index entry.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kIndexNotExist, std::move(err_msg))};
    }

    TableIndexEntry *header_index_entry = (TableIndexEntry *)header_base_entry;
    if (header_index_entry->Committed()) {
        if (begin_ts > header_index_entry->commit_ts_) {
            // No conflict
            if (header_index_entry->deleted_) {
                UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped before.");
                LOG_TRACE(*err_msg);
                return {nullptr, Status(ErrorCode::kIndexNotExist, std::move(err_msg))};
            }

            // Append new one to drop index
            auto table_index_entry = TableIndexEntry::NewDropTableIndexEntry(this, txn_id, begin_ts);
            table_index_entry_ptr = table_index_entry.get();
            table_index_entry_ptr->deleted_ = true;
            this->entry_list_.emplace_front(std::move(table_index_entry));
            return {table_index_entry_ptr, Status::OK()};
        } else {
            // Write-Write conflict
            UniquePtr<String> err_msg =
                MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
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
            UniquePtr<String> err_msg = MakeUnique<String>("Write-write conflict: There is another uncommitted table index entry.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
        }
    }
}

SharedPtr<String> TableIndexMeta::ToString() {
    UnrecoverableError("Not implemented");
    return nullptr;
}

nlohmann::json TableIndexMeta::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json_res;

    Vector<TableIndexEntry *> table_index_entry_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json_res["index_name"] = *this->index_name_;

        table_index_entry_candidates.reserve(this->entry_list_.size());
        for (const auto &base_entry : this->entry_list_) {
            if (base_entry->entry_type_ == EntryType::kDummy) {
                continue;
            }
            if (base_entry->entry_type_ != EntryType::kTableIndex) {
                UnrecoverableError("Unexpected entry type during serialize table index meta");
            }
            if (base_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                table_index_entry_candidates.push_back((TableIndexEntry *)base_entry.get());
            }
        }
    }

    for (const auto &table_index_entry : table_index_entry_candidates) {
        json_res["index_entries"].emplace_back(table_index_entry->Serialize(max_commit_ts));
    }
    return json_res;
}

UniquePtr<TableIndexMeta>
TableIndexMeta::Deserialize(const nlohmann::json &table_index_meta_json, TableEntry *table_entry, BufferManager *buffer_mgr) {
    LOG_TRACE(fmt::format("load index"));

    SharedPtr<String> index_name = MakeShared<String>(table_index_meta_json["index_name"]);
    auto res = MakeUnique<TableIndexMeta>(table_entry, index_name);
    if (table_index_meta_json.contains("index_entries")) {
        auto &entries = table_index_meta_json["index_entries"];
        // traverse reversely because a dummy head has been inserted
        for (auto iter = entries.rbegin(); iter != entries.rend(); iter++) {
            auto entry = TableIndexEntry::Deserialize(*iter, res.get(), buffer_mgr, table_entry);
            res->entry_list_.emplace_front(std::move(entry));
        }
    }
    return res;
}

Tuple<TableIndexEntry *, Status> TableIndexMeta::GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts) {

    TableIndexEntry *table_index_entry{nullptr};

    std::shared_lock<std::shared_mutex> r_locker(this->rw_locker_);
    for (const auto &entry : this->entry_list_) {
        if (entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
            LOG_ERROR(*err_msg);
            return {nullptr, Status::InvalidEntry()};
        }

        TransactionID entry_txn_id = entry->txn_id_.load();
        if (entry->commit_ts_ < UNCOMMIT_TS) {
            // committed
            if (begin_ts > entry->commit_ts_) {
                if (entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status::InvalidEntry()};
                } else {
                    table_index_entry = static_cast<TableIndexEntry *>(entry.get());
                    return {table_index_entry, Status::OK()};
                }
            }
        } else if (txn_id == entry_txn_id) {
            // same txn
            table_index_entry = static_cast<TableIndexEntry *>(entry.get());
            return {table_index_entry, Status::OK()};
        }
    }

    UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
    LOG_ERROR(*err_msg);
    return {nullptr, Status::InvalidEntry()};
}

Tuple<TableIndexEntry *, Status> TableIndexMeta::GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {

    if (!this->entry_list_.empty()) {
        const auto &entry = entry_list_.front();

        if (entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
            LOG_ERROR(*err_msg);
            return {nullptr, Status::InvalidEntry()};
        }

        TransactionID entry_txn_id = entry->txn_id_.load();
        // committed
        if (begin_ts > entry->commit_ts_) {
            if (entry->deleted_) {
                UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
                LOG_ERROR(*err_msg);
                return {nullptr, Status::InvalidEntry()};
            } else {
                auto table_index_entry = static_cast<TableIndexEntry *>(entry.get());
                return {table_index_entry, Status::OK()};
            }
        } else {
            if (txn_id == entry_txn_id) {
                auto table_index_entry = static_cast<TableIndexEntry *>(entry.get());
                return {table_index_entry, Status::OK()};
            }
        }
    }

    UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
    LOG_ERROR(*err_msg);
    return {nullptr, Status::InvalidEntry()};
}

void TableIndexMeta::DeleteNewEntry(TransactionID txn_id, TxnManager *) {
    std::unique_lock<std::shared_mutex> w_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        LOG_TRACE("Attempt to delete not existed entry.");
        return;
    }

    // `std::remove_if` move all elements that satisfy the predicate and move all the last element to the front of list. return value is the end of
    // the moved elements.
    auto removed_iter =
        std::remove_if(this->entry_list_.begin(), this->entry_list_.end(), [&](SharedPtr<BaseEntry> &entry) { return entry->txn_id_ == txn_id; });
    // erase the all "moved" elements in the end of list
    this->entry_list_.erase(removed_iter, this->entry_list_.end());
}

void TableIndexMeta::MergeFrom(TableIndexMeta &other) {
    if (!IsEqual(*this->index_name_, *other.index_name_)) {
        UnrecoverableError("TableIndexMeta::MergeFrom requires index_name_ match");
    }
    MergeLists(this->entry_list_, other.entry_list_);
}

} // namespace infinity

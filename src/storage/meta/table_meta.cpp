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

module catalog;

import :base_entry;

import stl;
import table_entry_type;
import parser;
import logger;
import default_values;
import third_party;
import txn_state;
import txn_manager;
import buffer_manager;

import third_party;
import status;
import infinity_exception;

namespace infinity {

/**
 * @brief Create a new table entry.
 *        LIST: [👇(insert a new).... table_entry2 , table_entry1 , dummy_entry] insert new table entry from front.
 * @param table_meta
 * @param table_entry_type
 * @param table_collection_name_ptr
 * @param columns
 * @param txn_id
 * @param begin_ts
 * @param txn_mgr
 * @return Status
 */
Tuple<TableEntry *, Status> TableMeta::CreateNewEntry(TableEntryType table_entry_type,
                                                      const SharedPtr<String> &table_collection_name_ptr,
                                                      const Vector<SharedPtr<ColumnDef>> &columns,
                                                      u64 txn_id,
                                                      TxnTimeStamp begin_ts,
                                                      TxnManager *txn_mgr) {
    UniqueLock<RWMutex> rw_locker(this->rw_locker_);
    const String &table_collection_name = *table_collection_name_ptr;

    TableEntry *table_entry_ptr{nullptr};

    if (this->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        this->entry_list_.emplace_back(Move(dummy_entry));

        // Insert the new table entry
        UniquePtr<TableEntry> table_entry =
            MakeUnique<TableEntry>(this->db_entry_dir_, table_collection_name_ptr, columns, table_entry_type, this, txn_id, begin_ts);
        table_entry_ptr = table_entry.get();
        this->entry_list_.emplace_front(Move(table_entry));

        LOG_TRACE(Format("New table entry is added: {}.", table_collection_name));
        return {table_entry_ptr, Status::OK()};
    } else {
        // Already have a table entry, check if the table entry is valid here.
        BaseEntry *header_base_entry = this->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            // Dummy entry in the header
            UniquePtr<TableEntry> table_entry =
                MakeUnique<TableEntry>(this->db_entry_dir_, table_collection_name_ptr, columns, table_entry_type, this, txn_id, begin_ts);
            table_entry_ptr = table_entry.get();
            this->entry_list_.emplace_front(Move(table_entry));
            return {table_entry_ptr, Status::OK()};
        }

        TableEntry *header_table_entry = (TableEntry *)header_base_entry;
        if (header_table_entry->commit_ts_ < UNCOMMIT_TS) {
            // Committed
            if (begin_ts > header_table_entry->commit_ts_) {
                if (header_table_entry->deleted_) {
                    // No conflict
                    UniquePtr<TableEntry> table_entry =
                        MakeUnique<TableEntry>(this->db_entry_dir_, table_collection_name_ptr, columns, table_entry_type, this, txn_id, begin_ts);
                    table_entry_ptr = table_entry.get();
                    this->entry_list_.emplace_front(Move(table_entry));
                    return {table_entry_ptr, Status::OK()};
                } else {
                    // Duplicated table
                    UniquePtr<String> err_msg = MakeUnique<String>(Format("Duplicated table: {}.", table_collection_name));
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kDuplicate, Move(err_msg))};
                }
            } else {
                // Write-Write conflict
                UniquePtr<String> err_msg = MakeUnique<String>(
                    Format("Write-write conflict: There is a committed table: {} which is later than current transaction.", table_collection_name));
                LOG_ERROR(*err_msg);
                return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
            }
        } else {

            TxnState head_table_entry_state = txn_mgr->GetTxnState(header_table_entry->txn_id_);

            switch (head_table_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if (header_table_entry->txn_id_ == txn_id) {
                        // Same txn
                        if (header_table_entry->deleted_) {
                            UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(this->db_entry_dir_,
                                                                                       table_collection_name_ptr,
                                                                                       columns,
                                                                                       table_entry_type,
                                                                                       this,
                                                                                       txn_id,
                                                                                       begin_ts);
                            table_entry_ptr = table_entry.get();
                            this->entry_list_.emplace_front(Move(table_entry));
                            return {table_entry_ptr, Status::OK()};
                        } else {
                            UniquePtr<String> err_msg = MakeUnique<String>(Format("Create a duplicated table {}.", table_collection_name));
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
                        Format("Write-write conflict: There is a committing/committed table which is later than current transaction."));
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    this->entry_list_.erase(this->entry_list_.begin());

                    // Append new one
                    UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(this->db_entry_dir_,
                                                                               table_collection_name_ptr,
                                                                               columns,
                                                                               table_entry_type,
                                                                               this,
                                                                               txn_id,
                                                                               begin_ts);
                    table_entry_ptr = table_entry.get();
                    this->entry_list_.emplace_front(Move(table_entry));
                    return {table_entry_ptr, Status::OK()};
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Invalid table entry txn state");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kUndefined, Move(err_msg))};
                }
            }
        }
    }
}

Tuple<TableEntry *, Status> TableMeta::DropNewEntry(u64 txn_id,
                                                    TxnTimeStamp begin_ts,
                                                    TxnManager *,
                                                    const String &table_name,
                                                    ConflictType conflict_type) {

    TableEntry *table_entry_ptr{nullptr};

    UniqueLock<RWMutex> rw_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty table entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    BaseEntry *header_base_entry = this->entry_list_.front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        UniquePtr<String> err_msg = MakeUnique<String>("No valid table entry.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    auto *header_table_entry = (TableEntry *)header_base_entry;
    if (header_table_entry->commit_ts_ < UNCOMMIT_TS) {
        // Committed
        if (begin_ts > header_table_entry->commit_ts_) {
            // No conflict
            if (header_table_entry->deleted_) {
                if (conflict_type == ConflictType::kIgnore) {
                    LOG_TRACE(Format("Ignore drop a not existed table entry {}", table_name));
                    return {nullptr, Status::OK()};
                }
                UniquePtr<String> err_msg = MakeUnique<String>("Table was dropped before.");
                LOG_ERROR(*err_msg);
                return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
            }

            Vector<SharedPtr<ColumnDef>> dummy_columns;
            UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(this->db_entry_dir_,
                                                                       this->table_name_,
                                                                       dummy_columns,
                                                                       TableEntryType::kTableEntry,
                                                                       this,
                                                                       txn_id,
                                                                       begin_ts);
            table_entry_ptr = table_entry.get();
            table_entry_ptr->deleted_ = true;
            this->entry_list_.emplace_front(Move(table_entry));

            return {table_entry_ptr, Status::OK()};
        } else {
            // Write-Write conflict
            UniquePtr<String> err_msg =
                MakeUnique<String>(Format("Write-write conflict: There is a committed database which is later than current transaction."));
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_table_entry->txn_id_) {
            // Same txn, remove the header table entry
            table_entry_ptr = header_table_entry;
            this->entry_list_.erase(this->entry_list_.begin());

            return {table_entry_ptr, Status::OK()};
        } else {
            // Not same txn, issue WW conflict
            UniquePtr<String> err_msg = MakeUnique<String>(Format("Write-write conflict: There is another uncommitted table entry."));
            LOG_ERROR(*err_msg);
            return {table_entry_ptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
        }
    }
}

void TableMeta::DeleteNewEntry(u64 txn_id, TxnManager *) {
    UniqueLock<RWMutex> rw_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        LOG_TRACE("Empty table entry list.");
        return;
    }

    auto removed_iter = std::remove_if(this->entry_list_.begin(), this->entry_list_.end(), [&](UniquePtr<BaseEntry> &entry) -> bool {
        return entry->txn_id_ == txn_id;
    });

    this->entry_list_.erase(removed_iter, this->entry_list_.end());
}

/**
 * @brief Get the table entry object
 *        LIST: [👇(a new entry).... table_entry2 , table_entry1 , dummy_entry]
 *        Get the first valid table entry from the front.
 *        The table entry is valid if:
 *          1. committed and commit_ts < begin_ts.
 *          2. not dummy entry.
 * @param table_meta
 * @param txn_id
 * @param begin_ts
 * @return Status
 */
Tuple<TableEntry *, Status> TableMeta::GetEntry(u64 txn_id, TxnTimeStamp begin_ts) {
    SharedLock<RWMutex> r_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty table entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    for (const auto &table_entry : this->entry_list_) {
        if (table_entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid table entry. dummy entry");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
        }

        u64 table_entry_commit_ts = table_entry->commit_ts_;
        if (table_entry_commit_ts < UNCOMMIT_TS) {
            // committed
            if (begin_ts > table_entry_commit_ts) {
                if (table_entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("Table was dropped.");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
                } else {
                    return {static_cast<TableEntry *>(table_entry.get()), Status::OK()};
                }
            }
        } else {
            // not committed
            if (txn_id == table_entry->txn_id_) {
                // same txn
                return {static_cast<TableEntry *>(table_entry.get()), Status::OK()};
            }
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>("No table entry found.");
    LOG_ERROR(*err_msg);
    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
}

const SharedPtr<String>& TableMeta::db_name_ptr() const {
    return db_entry_->db_name_ptr();
}

SharedPtr<String> TableMeta::ToString(TableMeta *table_meta) {
    SharedLock<RWMutex> r_locker(table_meta->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(Format("TableMeta, db_entry_dir: {}, table name: {}, entry count: ",
                                                      *table_meta->db_entry_dir_,
                                                      *table_meta->table_name_,
                                                      table_meta->entry_list_.size()));
    return res;
}

Json TableMeta::Serialize(TableMeta *table_meta, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;
    Vector<TableEntry *> table_candidates;
    {
        SharedLock<RWMutex> lck(table_meta->rw_locker_);
        json_res["db_entry_dir"] = *table_meta->db_entry_dir_;
        json_res["table_name"] = *table_meta->table_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        table_candidates.reserve(table_meta->entry_list_.size());
        for (auto &table_entry : table_meta->entry_list_) {
            if (table_entry->entry_type_ == EntryType::kTable && table_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                table_candidates.push_back((TableEntry *)table_entry.get());
            }
        }
    }
    for (TableEntry *table_entry : table_candidates) {
        json_res["table_entries"].emplace_back(TableEntry::Serialize(table_entry, max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

/**
 * @brief Deserialize the table meta from json.
 *        The table meta is a list of table entries in reverse order.
 *        Same as CreateNewEntry, the last entry is a dummy entry.
 *        LIST: [👇(a new entry).... table_entry2 , table_entry1 , dummy_entry]
 *        The raw catalog is json, so the dummy entry is not included.
 *        The dummy entry is added during the deserialization.
 * @param table_meta_json
 * @param db_entry
 * @param buffer_mgr
 * @return UniquePtr<TableMeta>
 */
UniquePtr<TableMeta> TableMeta::Deserialize(const Json &table_meta_json, DBEntry *db_entry, BufferManager *buffer_mgr) {
    SharedPtr<String> db_entry_dir = MakeShared<String>(table_meta_json["db_entry_dir"]);
    SharedPtr<String> table_name = MakeShared<String>(table_meta_json["table_name"]);
    LOG_TRACE(Format("load table {}", *table_name));
    UniquePtr<TableMeta> res = MakeUnique<TableMeta>(db_entry_dir, table_name, db_entry);
    if (table_meta_json.contains("table_entries")) {
        for (const auto &table_entry_json : table_meta_json["table_entries"]) {
            UniquePtr<TableEntry> table_entry = TableEntry::Deserialize(table_entry_json, res.get(), buffer_mgr);
            res->entry_list_.emplace_back(Move(table_entry));
        }
    }
    res->entry_list_.sort([](const UniquePtr<BaseEntry> &ent1, const UniquePtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });
    UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
    dummy_entry->deleted_ = true;
    res->entry_list_.emplace_back(Move(dummy_entry));

    return res;
}

void TableMeta::MergeFrom(TableMeta &other) {
    // No locking here since only the load stage needs MergeFrom.
    if (!IsEqual(*this->table_name_, *other.table_name_) || !IsEqual(*this->db_entry_dir_, *other.db_entry_dir_)) {
        Error<StorageException>("DBEntry::MergeFrom requires table_name_ and db_entry_dir_ match");
    }
    MergeLists(this->entry_list_, other.entry_list_);
}

} // namespace infinity

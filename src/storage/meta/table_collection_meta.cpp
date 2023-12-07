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

import base_entry;
import stl;
import table_collection_entry;
import table_collection_type;
import parser;
import logger;
import default_values;
import third_party;
import txn_state;
import txn_manager;
import buffer_manager;
import db_entry;
import third_party;
import status;
import infinity_exception;

module table_collection_meta;

namespace infinity {

/**
 * @brief Create a new table entry.
 *        LIST: [👇(insert a new).... table_entry2 , table_entry1 , dummy_entry] insert new table entry from front.
 * @param table_meta
 * @param table_collection_type
 * @param table_collection_name_ptr
 * @param columns
 * @param txn_id
 * @param begin_ts
 * @param txn_mgr
 * @return Status
 */
Status TableCollectionMeta::CreateNewEntry(TableCollectionMeta *table_meta,
                                           TableCollectionType table_collection_type,
                                           const SharedPtr<String> &table_collection_name_ptr,
                                           const Vector<SharedPtr<ColumnDef>> &columns,
                                           u64 txn_id,
                                           TxnTimeStamp begin_ts,
                                           TxnManager *txn_mgr,
                                           BaseEntry *&base_entry) {
    UniqueLock<RWMutex> rw_locker(table_meta->rw_locker_);
    const String &table_collection_name = *table_collection_name_ptr;

    if (table_meta->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        table_meta->entry_list_.emplace_back(Move(dummy_entry));

        // Insert the new table entry
        UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                       table_collection_name_ptr,
                                                                                       columns,
                                                                                       table_collection_type,
                                                                                       table_meta,
                                                                                       txn_id,
                                                                                       begin_ts);
        base_entry = table_entry.get();
        table_meta->entry_list_.emplace_front(Move(table_entry));

        LOG_TRACE(Format("New table entry is added: {}.", table_collection_name));
        return Status::OK();
    } else {
        // Already have a table entry, check if the table entry is valid here.
        BaseEntry *header_base_entry = table_meta->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            // Dummy entry in the header
            UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                           table_collection_name_ptr,
                                                                                           columns,
                                                                                           table_collection_type,
                                                                                           table_meta,
                                                                                           txn_id,
                                                                                           begin_ts);
            base_entry = table_entry.get();
            table_meta->entry_list_.emplace_front(Move(table_entry));
            return Status::OK();
        }

        TableCollectionEntry *header_table_entry = (TableCollectionEntry *)header_base_entry;
        if (header_table_entry->commit_ts_ < UNCOMMIT_TS) {
            // Committed
            if (begin_ts > header_table_entry->commit_ts_) {
                if (header_table_entry->deleted_) {
                    // No conflict
                    UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                                   table_collection_name_ptr,
                                                                                                   columns,
                                                                                                   table_collection_type,
                                                                                                   table_meta,
                                                                                                   txn_id,
                                                                                                   begin_ts);
                    base_entry = table_entry.get();
                    table_meta->entry_list_.emplace_front(Move(table_entry));
                    return Status::OK();
                } else {
                    // Duplicated table
                    UniquePtr<String> err_msg = MakeUnique<String>(Format("Duplicated table: {}.", table_collection_name));
                    LOG_ERROR(*err_msg);
                    return Status(ErrorCode::kDuplicate, Move(err_msg));
                }
            } else {
                // Write-Write conflict
                UniquePtr<String> err_msg = MakeUnique<String>(
                    Format("Write-write conflict: There is a committed table: {} which is later than current transaction.", table_collection_name));
                LOG_ERROR(*err_msg);
                return Status(ErrorCode::kWWConflict, Move(err_msg));
            }
        } else {

            TxnState head_table_entry_state = txn_mgr->GetTxnState(header_table_entry->txn_id_);

            switch (head_table_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if (header_table_entry->txn_id_ == txn_id) {
                        // Same txn
                        if (header_table_entry->deleted_) {
                            UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                                           table_collection_name_ptr,
                                                                                                           columns,
                                                                                                           table_collection_type,
                                                                                                           table_meta,
                                                                                                           txn_id,
                                                                                                           begin_ts);
                            base_entry = table_entry.get();
                            table_meta->entry_list_.emplace_front(Move(table_entry));
                            return Status::OK();
                        } else {
                            UniquePtr<String> err_msg = MakeUnique<String>(Format("Create a duplicated table {}.", table_collection_name));
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
                        Format("Write-write conflict: There is a committing/committed table which is later than current transaction."));
                    LOG_ERROR(*err_msg);
                    return Status(ErrorCode::kWWConflict, Move(err_msg));
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    table_meta->entry_list_.erase(table_meta->entry_list_.begin());

                    // Append new one
                    UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                                   table_collection_name_ptr,
                                                                                                   columns,
                                                                                                   table_collection_type,
                                                                                                   table_meta,
                                                                                                   txn_id,
                                                                                                   begin_ts);
                    base_entry = table_entry.get();
                    table_meta->entry_list_.emplace_front(Move(table_entry));
                    return Status::OK();
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Invalid table entry txn state");
                    LOG_ERROR(*err_msg);
                    return Status(ErrorCode::kUndefined, Move(err_msg));
                }
            }
        }
    }
}

Status TableCollectionMeta::DropNewEntry(TableCollectionMeta *table_meta,
                                         u64 txn_id,
                                         TxnTimeStamp begin_ts,
                                         TxnManager *,
                                         const String &table_name,
                                         ConflictType conflict_type,
                                         BaseEntry *&base_entry) {
    UniqueLock<RWMutex> rw_locker(table_meta->rw_locker_);
    if (table_meta->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty table entry list.");
        LOG_ERROR(*err_msg);
        return Status(ErrorCode::kNotFound, Move(err_msg));
    }

    BaseEntry *header_base_entry = table_meta->entry_list_.front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        UniquePtr<String> err_msg = MakeUnique<String>("No valid table entry.");
        LOG_ERROR(*err_msg);
        return Status(ErrorCode::kNotFound, Move(err_msg));
    }

    auto *header_table_entry = (TableCollectionEntry *)header_base_entry;
    if (header_table_entry->commit_ts_ < UNCOMMIT_TS) {
        // Committed
        if (begin_ts > header_table_entry->commit_ts_) {
            // No conflict
            if (header_table_entry->deleted_) {
                if (conflict_type == ConflictType::kIgnore) {
                    LOG_TRACE(Format("Ignore drop a not existed table entry {}", table_name));
                    return Status::OK();
                }
                UniquePtr<String> err_msg = MakeUnique<String>("Table was dropped before.");
                LOG_ERROR(*err_msg);
                return Status(ErrorCode::kNotFound, Move(err_msg));
            }

            Vector<SharedPtr<ColumnDef>> dummy_columns;
            UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                           table_meta->table_collection_name_,
                                                                                           dummy_columns,
                                                                                           TableCollectionType::kTableEntry,
                                                                                           table_meta,
                                                                                           txn_id,
                                                                                           begin_ts);
            base_entry = table_entry.get();
            base_entry->deleted_ = true;
            table_meta->entry_list_.emplace_front(Move(table_entry));

            return Status::OK();
        } else {
            // Write-Write conflict
            UniquePtr<String> err_msg =
                MakeUnique<String>(Format("Write-write conflict: There is a committed database which is later than current transaction."));
            LOG_ERROR(*err_msg);
            return Status(ErrorCode::kWWConflict, Move(err_msg));
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_table_entry->txn_id_) {
            // Same txn, remove the header table entry
            base_entry = header_table_entry;
            table_meta->entry_list_.erase(table_meta->entry_list_.begin());

            return Status::OK();
        } else {
            // Not same txn, issue WW conflict
            UniquePtr<String> err_msg = MakeUnique<String>(Format("Write-write conflict: There is another uncommitted table entry."));
            LOG_ERROR(*err_msg);
            return Status(ErrorCode::kWWConflict, Move(err_msg));
        }
    }
}

void TableCollectionMeta::DeleteNewEntry(TableCollectionMeta *table_meta, u64 txn_id, TxnManager *) {
    UniqueLock<RWMutex> rw_locker(table_meta->rw_locker_);
    if (table_meta->entry_list_.empty()) {
        LOG_TRACE("Empty table entry list.");
        return;
    }

    auto removed_iter = std::remove_if(table_meta->entry_list_.begin(), table_meta->entry_list_.end(), [&](UniquePtr<BaseEntry> &entry) -> bool {
        return entry->txn_id_ == txn_id;
    });

    table_meta->entry_list_.erase(removed_iter, table_meta->entry_list_.end());
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
Tuple<BaseEntry*, Status> TableCollectionMeta::GetEntry(TableCollectionMeta *table_meta, u64 txn_id, TxnTimeStamp begin_ts) {
    SharedLock<RWMutex> r_locker(table_meta->rw_locker_);
    if (table_meta->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty table entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    for (const auto &table_entry : table_meta->entry_list_) {
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
                    return {table_entry.get(), Status::OK()};
                }
            }
        } else {
            // not committed
            if (txn_id == table_entry->txn_id_) {
                // same txn
                return {table_entry.get(), Status::OK()};
            }
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>("No table entry found.");
    LOG_ERROR(*err_msg);
    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
}

SharedPtr<String> TableCollectionMeta::ToString(TableCollectionMeta *table_meta) {
    SharedLock<RWMutex> r_locker(table_meta->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(Format("TableMeta, db_entry_dir: {}, table name: {}, entry count: ",
                                                      *table_meta->db_entry_dir_,
                                                      *table_meta->table_collection_name_,
                                                      table_meta->entry_list_.size()));
    return res;
}

Json TableCollectionMeta::Serialize(TableCollectionMeta *table_meta, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;
    Vector<TableCollectionEntry *> table_candidates;
    {
        SharedLock<RWMutex> lck(table_meta->rw_locker_);
        json_res["db_entry_dir"] = *table_meta->db_entry_dir_;
        json_res["table_name"] = *table_meta->table_collection_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        table_candidates.reserve(table_meta->entry_list_.size());
        for (auto &table_entry : table_meta->entry_list_) {
            if (table_entry->entry_type_ == EntryType::kTable && table_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                table_candidates.push_back((TableCollectionEntry *)table_entry.get());
            }
        }
    }
    for (TableCollectionEntry *table_entry : table_candidates) {
        json_res["table_entries"].emplace_back(TableCollectionEntry::Serialize(table_entry, max_commit_ts, is_full_checkpoint));
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
 * @return UniquePtr<TableCollectionMeta>
 */
UniquePtr<TableCollectionMeta> TableCollectionMeta::Deserialize(const Json &table_meta_json, DBEntry *db_entry, BufferManager *buffer_mgr) {
    SharedPtr<String> db_entry_dir = MakeShared<String>(table_meta_json["db_entry_dir"]);
    SharedPtr<String> table_name = MakeShared<String>(table_meta_json["table_name"]);
    LOG_TRACE(Format("load table {}", *table_name));
    UniquePtr<TableCollectionMeta> res = MakeUnique<TableCollectionMeta>(db_entry_dir, table_name, db_entry);
    if (table_meta_json.contains("table_entries")) {
        for (const auto &table_entry_json : table_meta_json["table_entries"]) {
            UniquePtr<TableCollectionEntry> table_entry = TableCollectionEntry::Deserialize(table_entry_json, res.get(), buffer_mgr);
            res->entry_list_.emplace_back(Move(table_entry));
        }
    }
    res->entry_list_.sort([](const UniquePtr<BaseEntry> &ent1, const UniquePtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });
    UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
    dummy_entry->deleted_ = true;
    res->entry_list_.emplace_back(Move(dummy_entry));

    return res;
}

void TableCollectionMeta::MergeFrom(TableCollectionMeta &other) {
    // No locking here since only the load stage needs MergeFrom.
    Assert<StorageException>(IsEqual(*this->table_collection_name_, *other.table_collection_name_),
                             "DBEntry::MergeFrom requires table_collection_name_ match");
    Assert<StorageException>(IsEqual(*this->db_entry_dir_, *other.db_entry_dir_), "DBEntry::MergeFrom requires db_entry_dir_ match");
    MergeLists(this->entry_list_, other.entry_list_);
}

} // namespace infinity

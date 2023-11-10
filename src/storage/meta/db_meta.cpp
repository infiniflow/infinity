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
import db_entry;
import txn_manager;
import logger;
import third_party;
import buffer_manager;
import txn_state;

import infinity_exception;

module db_meta;

namespace infinity {

EntryResult DBMeta::CreateNewEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    DBEntry *res = nullptr;
    UniqueLock<RWMutex> rw_locker(db_meta->rw_locker_);

    //    rw_locker_.lock();
    if (db_meta->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        db_meta->entry_list_.emplace_back(Move(dummy_entry));

        // Insert the new db entry
        UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir_, db_meta->db_name_, txn_id, begin_ts);
        res = db_entry.get();
        db_meta->entry_list_.emplace_front(Move(db_entry));

        LOG_TRACE("New database entry is added.");
        return {res, nullptr};
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry *header_base_entry = db_meta->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir_, db_meta->db_name_, txn_id, begin_ts);
            res = db_entry.get();
            db_meta->entry_list_.emplace_front(Move(db_entry));
            return {res, nullptr};
        }

        DBEntry *header_db_entry = (DBEntry *)header_base_entry;
        if (header_db_entry->Committed()) {
            if (begin_ts > header_db_entry->commit_ts_) {
                if (header_db_entry->deleted_) {
                    // No conflict
                    UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir_, db_meta->db_name_, txn_id, begin_ts);
                    res = db_entry.get();
                    db_meta->entry_list_.emplace_front(Move(db_entry));
                    return {res, nullptr};
                } else {
                    // Duplicated database
                    LOG_ERROR(Format("Duplicated database name: {}.", *db_meta->db_name_));
                    return {nullptr, MakeUnique<String>("Duplicated database.")};
                }
            } else {
                // Write-Write conflict
                LOG_TRACE(Format("Write-write conflict: There is a committed database which is later than current transaction."));
                return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
            }
        } else {

            TxnState head_db_entry_state = txn_mgr->GetTxnState(header_db_entry->txn_id_);

            switch (head_db_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if (header_db_entry->txn_id_ == txn_id) {
                        // Same txn
                        if (header_db_entry->deleted_) {
                            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir_, db_meta->db_name_, txn_id, begin_ts);
                            res = db_entry.get();
                            db_meta->entry_list_.emplace_front(Move(db_entry));
                            return {res, nullptr};
                        } else {
                            LOG_TRACE("Create a duplicated name database.");
                            return {nullptr, MakeUnique<String>("Create a duplicated name database.")};
                        }
                    } else {
                        LOG_TRACE("Write-write conflict: There is a uncommitted transaction.");
                        return {nullptr, MakeUnique<String>("Write-write conflict: There is a uncommitted transaction.")};
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    LOG_TRACE("Write-write conflict: There is a committing/committed database which is later than current transaction.");
                    return {nullptr,
                            MakeUnique<String>(
                                "Write-write conflict: There is a committing/committed database which is later than current transaction.")};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    db_meta->entry_list_.erase(db_meta->entry_list_.begin());

                    // Append new one
                    UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir_, db_meta->db_name_, txn_id, begin_ts);
                    res = db_entry.get();
                    db_meta->entry_list_.emplace_front(Move(db_entry));
                    return {res, nullptr};
                }
                default: {
                    LOG_TRACE("Invalid db entry txn state");
                    return {nullptr, MakeUnique<String>("Invalid db entry txn state.")};
                }
            }
        }
    }
}

EntryResult DBMeta::DropNewEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    DBEntry *res = nullptr;
    UniqueLock<RWMutex> rw_locker(db_meta->rw_locker_);
    if (db_meta->entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.");
        return {nullptr, MakeUnique<String>("Empty db entry list.")};
    }

    BaseEntry *header_base_entry = db_meta->entry_list_.front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        //            rw_locker_.unlock();
        LOG_TRACE("No valid db entry.");
        return {nullptr, MakeUnique<String>("No valid db entry.")};
    }

    DBEntry *header_db_entry = (DBEntry *)header_base_entry;
    if (header_db_entry->Committed()) {
        if (begin_ts > header_db_entry->commit_ts_) {
            // No conflict
            if (header_db_entry->deleted_) {
                LOG_TRACE("DB is dropped before.");
                return {nullptr, MakeUnique<String>("DB is dropped before.")};
            }

            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->data_dir_, db_meta->db_name_, txn_id, begin_ts);
            res = db_entry.get();
            res->deleted_ = true;
            db_meta->entry_list_.emplace_front(Move(db_entry));

            return {res, nullptr};
        } else {
            // Write-Write conflict
            LOG_TRACE("Write-write conflict: There is a committed database which is later than current transaction.");
            return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_db_entry->txn_id_) {
            // Same txn, remove the header db entry
            res = header_db_entry;
            db_meta->entry_list_.erase(db_meta->entry_list_.begin());

            return {res, nullptr};
        } else {
            // Not same txn, issue WW conflict
            LOG_TRACE("Write-write conflict: There is another uncommitted db entry.");
            return {nullptr, MakeUnique<String>("Write-write conflict: There is another uncommitted db entry.")};
        }
    }
}

void DBMeta::DeleteNewEntry(DBMeta *db_meta, u64 txn_id, TxnManager *txn_mgr) {
    UniqueLock<RWMutex> rw_locker(db_meta->rw_locker_);
    if (db_meta->entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.");
        return;
    }

    auto removed_iter =
        std::remove_if(db_meta->entry_list_.begin(), db_meta->entry_list_.end(), [&](auto &entry) -> bool { return entry->txn_id_ == txn_id; });

    db_meta->entry_list_.erase(removed_iter, db_meta->entry_list_.end());
}

EntryResult DBMeta::GetEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts) {
    SharedLock<RWMutex> r_locker(db_meta->rw_locker_);
    if (db_meta->entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.");
        return {nullptr, MakeUnique<String>("Empty db entry list.")};
    }

    for (const auto &db_entry : db_meta->entry_list_) {
        if (db_entry->entry_type_ == EntryType::kDummy) {
            LOG_TRACE("No valid db entry.");
            return {nullptr, MakeUnique<String>("No valid db entry.")};
        }

        if (db_entry->Committed()) {
            if (begin_ts > db_entry->commit_ts_) {
                if (db_entry->deleted_) {
                    //                    LOG_TRACE("DB is dropped.")
                    return {nullptr, MakeUnique<String>("DB is dropped.")};
                } else {
                    // check the tables meta
                    return {db_entry.get(), nullptr};
                }
            }
        } else {
            // Only committed txn is visible. Committing txn isn't visble,
            // except same txn is visible
            if (txn_id == db_entry->txn_id_) {
                if (db_entry->deleted_) {
                    //                    LOG_TRACE("DB is dropped.")
                    return {nullptr, MakeUnique<String>("DB is dropped.")};
                } else {
                    return {db_entry.get(), nullptr};
                }
            }
        }
    }
    LOG_TRACE("No db entry found.");
    return {nullptr, MakeUnique<String>("No db entry found.")};
}

SharedPtr<String> DBMeta::ToString(DBMeta *db_meta) {
    SharedLock<RWMutex> r_locker(db_meta->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(
        Format("DBMeta, data_dir: {}, db name: {}, entry count: ", *db_meta->data_dir_, *db_meta->db_name_, db_meta->entry_list_.size()));
    return res;
}

Json DBMeta::Serialize(DBMeta *db_meta, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;
    Vector<DBEntry *> db_entries;
    {
        SharedLock<RWMutex> lck(db_meta->rw_locker_);
        json_res["data_dir"] = *db_meta->data_dir_;
        json_res["db_name"] = *db_meta->db_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        for (auto &db_entry : db_meta->entry_list_) {
            if (db_entry->entry_type_ == EntryType::kDatabase && db_entry->Committed() && db_entry->commit_ts_ <= max_commit_ts)
                db_entries.push_back((DBEntry *)db_entry.get());
        }
    }
    for (DBEntry *db_entry : db_entries) {
        json_res["entries"].emplace_back(DBEntry::Serialize(db_entry, max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

UniquePtr<DBMeta> DBMeta::Deserialize(const Json &db_meta_json, BufferManager *buffer_mgr) {
    SharedPtr<String> data_dir = MakeShared<String>(db_meta_json["data_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_meta_json["db_name"]);
    UniquePtr<DBMeta> res = MakeUnique<DBMeta>(data_dir, db_name);

    if (db_meta_json.contains("entries")) {
        for (const auto &db_entry_json : db_meta_json["entries"]) {
            res->entry_list_.emplace_back(DBEntry::Deserialize(db_entry_json, buffer_mgr));
        }
    }
    res->entry_list_.sort([](const UniquePtr<BaseEntry> &ent1, const UniquePtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });
    return res;
}

void DBMeta::MergeFrom(DBMeta &other) {
    // No locking here since only the load stage needs MergeFrom.
    Assert<StorageException>(IsEqual(*this->db_name_, *other.db_name_), "DBMeta::MergeFrom requires db_name_ match");
    Assert<StorageException>(IsEqual(*this->data_dir_, *other.data_dir_), "DBMeta::MergeFrom requires db_dir_ match");
    MergeLists(this->entry_list_, other.entry_list_);
}

} // namespace infinity

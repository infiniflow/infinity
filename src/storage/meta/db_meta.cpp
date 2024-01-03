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
import parser;
import txn_manager;
import logger;
import third_party;
import buffer_manager;
import txn_state;
import status;
import infinity_exception;

namespace infinity {

Tuple<DBEntry *, Status> DBMeta::CreateNewEntry(u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflic_type) {
    UniqueLock<RWMutex> rw_locker(this->rw_locker_);
    DBEntry *db_entry_ptr{nullptr};
    //    rw_locker_.lock();
    if (this->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        this->entry_list_.emplace_back(Move(dummy_entry));

        // Insert the new db entry
        UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(this->data_dir_, this->db_name_, txn_id, begin_ts);
        db_entry_ptr = db_entry.get();
        this->entry_list_.emplace_front(Move(db_entry));

        LOG_TRACE("New database entry is added.");
        return {db_entry_ptr, Status::OK()};
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry *header_base_entry = this->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(this->data_dir_, this->db_name_, txn_id, begin_ts);
            db_entry_ptr = db_entry.get();
            this->entry_list_.emplace_front(Move(db_entry));
            return {db_entry_ptr, Status::OK()};
        }

        DBEntry *header_db_entry = (DBEntry *)header_base_entry;
        if (header_db_entry->Committed()) {
            if (begin_ts > header_db_entry->commit_ts_) {
                if (header_db_entry->deleted_) {
                    // No conflict
                    UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(this->data_dir_, this->db_name_, txn_id, begin_ts);
                    db_entry_ptr = db_entry.get();
                    this->entry_list_.emplace_front(Move(db_entry));
                    return {db_entry_ptr, Status::OK()};
                } else {
                    switch (conflic_type) {
                        case ConflictType::kIgnore: {
                            db_entry_ptr = header_db_entry;
                            return {db_entry_ptr, Status::OK()};
                        }
                        default: {
                            // Duplicated database
                            UniquePtr<String> err_msg = MakeUnique<String>(Format("Duplicated database name: {}.", *this->db_name_));
                            LOG_ERROR(*err_msg);
                            return {db_entry_ptr, Status(ErrorCode::kDuplicate, Move(err_msg))};
                        }
                    }
                }
            } else {
                // Write-Write conflict
                UniquePtr<String> err_msg =
                    MakeUnique<String>(Format("Write-write conflict: There is a committed database which is later than current transaction."));
                LOG_ERROR(*err_msg);
                return {db_entry_ptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
            }
        } else {

            TxnState head_db_entry_state = txn_mgr->GetTxnState(header_db_entry->txn_id_);

            switch (head_db_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if (header_db_entry->txn_id_ == txn_id) {
                        // Same txn
                        if (header_db_entry->deleted_) {
                            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(this->data_dir_, this->db_name_, txn_id, begin_ts);
                            db_entry_ptr = db_entry.get();
                            this->entry_list_.emplace_front(Move(db_entry));
                            return {db_entry_ptr, Status::OK()};
                        } else {
                            UniquePtr<String> err_msg = MakeUnique<String>(Format("Duplicated database name: {}.", *this->db_name_));
                            LOG_ERROR(*err_msg);
                            return {db_entry_ptr, Status(ErrorCode::kDuplicate, Move(err_msg))};
                        }
                    } else {
                        UniquePtr<String> err_msg = MakeUnique<String>(Format("Write-write conflict: There is a uncommitted transaction."));
                        LOG_ERROR(*err_msg);
                        return {db_entry_ptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    UniquePtr<String> err_msg = MakeUnique<String>(
                        Format("Write-write conflict: There is a committing/committed database which is later than current transaction."));
                    LOG_ERROR(*err_msg);
                    return {db_entry_ptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    this->entry_list_.erase(this->entry_list_.begin());

                    // Append new one
                    UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(this->data_dir_, this->db_name_, txn_id, begin_ts);
                    db_entry_ptr = db_entry.get();
                    this->entry_list_.emplace_front(Move(db_entry));
                    return {db_entry_ptr, Status::OK()};
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Invalid db entry txn state");
                    LOG_ERROR(*err_msg);
                    return {db_entry_ptr, Status(ErrorCode::kUndefined, Move(err_msg))};
                }
            }
        }
    }
}

Tuple<DBEntry *, Status> DBMeta::DropNewEntry(u64 txn_id, TxnTimeStamp begin_ts, TxnManager *) {

    DBEntry *db_entry_ptr{nullptr};
    UniqueLock<RWMutex> rw_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty db entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    BaseEntry *header_base_entry = this->entry_list_.front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        UniquePtr<String> err_msg = MakeUnique<String>("No valid db entry.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    DBEntry *header_db_entry = (DBEntry *)header_base_entry;
    if (header_db_entry->Committed()) {
        if (begin_ts > header_db_entry->commit_ts_) {
            // No conflict
            if (header_db_entry->deleted_) {
                UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped before.");
                LOG_TRACE(*err_msg);
                return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
            }

            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(this->data_dir_, this->db_name_, txn_id, begin_ts);
            db_entry_ptr = db_entry.get();
            db_entry_ptr->deleted_ = true;
            this->entry_list_.emplace_front(Move(db_entry));

            return {db_entry_ptr, Status::OK()};
        } else {
            // Write-Write conflict
            UniquePtr<String> err_msg =
                MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_db_entry->txn_id_) {
            // Same txn, remove the header db entry
            db_entry_ptr = header_db_entry;
            this->entry_list_.erase(this->entry_list_.begin());

            return {db_entry_ptr, Status::OK()};
        } else {
            // Not same txn, issue WW conflict
            UniquePtr<String> err_msg = MakeUnique<String>("Write-write conflict: There is another uncommitted db entry.");
            LOG_ERROR(*err_msg);
            return {db_entry_ptr, Status(ErrorCode::kWWConflict, Move(err_msg))};
        }
    }
}

void DBMeta::AddEntry(DBMeta *db_meta, UniquePtr<BaseEntry> db_entry) {
    UniqueLock<RWMutex> rw_locker(db_meta->rw_locker_);
    db_meta->entry_list_.emplace_front(Move(db_entry));
}

void DBMeta::DeleteNewEntry(u64 txn_id, TxnManager *) {
    UniqueLock<RWMutex> rw_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.");
        return;
    }

    auto removed_iter =
        std::remove_if(this->entry_list_.begin(), this->entry_list_.end(), [&](auto &entry) -> bool { return entry->txn_id_ == txn_id; });

    this->entry_list_.erase(removed_iter, this->entry_list_.end());
}

Tuple<DBEntry *, Status> DBMeta::GetEntry(u64 txn_id, TxnTimeStamp begin_ts) {
    SharedLock<RWMutex> r_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty db entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    for (const auto &db_entry : this->entry_list_) {
        if (db_entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid db entry.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
        }

        if (db_entry->Committed()) {
            if (begin_ts > db_entry->commit_ts_) {
                if (db_entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped.");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
                } else {
                    // check the tables meta
                    return {(DBEntry*)(db_entry.get()), Status::OK()};
                }
            }
        } else {
            // Only committed txn is visible. Committing txn isn't visble,
            // except same txn is visible
            if (txn_id == db_entry->txn_id_) {
                if (db_entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped.");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
                } else {
                    return {(DBEntry*)(db_entry.get()), Status::OK()};
                }
            }
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>("No db entry found.");
    LOG_ERROR(*err_msg);
    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
}

SharedPtr<String> DBMeta::ToString(DBMeta *db_meta) {
    SharedLock<RWMutex> r_locker(db_meta->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(
        Format("DBMeta, data_dir: {}, db name: {}, entry count: ", *db_meta->data_dir_, *db_meta->db_name_, db_meta->entry_list_.size()));
    return res;
}

Json DBMeta::Serialize(DBMeta *db_meta, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;
    Vector<DBEntry *> db_candidates;
    {
        SharedLock<RWMutex> lck(db_meta->rw_locker_);
        json_res["data_dir"] = *db_meta->data_dir_;
        json_res["db_name"] = *db_meta->db_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        db_candidates.reserve(db_meta->entry_list_.size());
        for (auto &db_entry : db_meta->entry_list_) {
            if (db_entry->entry_type_ == EntryType::kDatabase && db_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                db_candidates.push_back((DBEntry *)db_entry.get());
            }
        }
    }
    for (DBEntry *db_entry : db_candidates) {
        json_res["db_entries"].emplace_back(DBEntry::Serialize(db_entry, max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

UniquePtr<DBMeta> DBMeta::Deserialize(const Json &db_meta_json, BufferManager *buffer_mgr) {
    SharedPtr<String> data_dir = MakeShared<String>(db_meta_json["data_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_meta_json["db_name"]);
    UniquePtr<DBMeta> res = MakeUnique<DBMeta>(data_dir, db_name);

    if (db_meta_json.contains("db_entries")) {
        for (const auto &db_entry_json : db_meta_json["db_entries"]) {
            res->entry_list_.emplace_back(DBEntry::Deserialize(db_entry_json, buffer_mgr));
        }
    }
    res->entry_list_.sort([](const UniquePtr<BaseEntry> &ent1, const UniquePtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });
    return res;
}

void DBMeta::MergeFrom(DBMeta &other) {
    // No locking here since only the load stage needs MergeFrom.
    if (!IsEqual(*this->db_name_, *other.db_name_)) {
        Error<StorageException>("DBMeta::MergeFrom requires db_name_ match");
    }
    if (!IsEqual(*this->data_dir_, *other.data_dir_)) {
        Error<StorageException>("DBMeta::MergeFrom requires db_dir_ match");
    }
    MergeLists(this->entry_list_, other.entry_list_);
}

} // namespace infinity

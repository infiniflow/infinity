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

import :base_entry;

import stl;
import parser;
import txn_manager;
import logger;
import third_party;
import buffer_manager;
import txn_state;
import status;
import catalog_delta_entry;
import infinity_exception;

namespace infinity {

// Use by default
UniquePtr<DBMeta> DBMeta::NewDBMeta(const SharedPtr<String> &data_dir, const SharedPtr<String> &db_name) {
    auto db_meta = MakeUnique<DBMeta>(data_dir, db_name);
    return db_meta;
}

// TODO: Use Txn* txn as parma instead of TransactionID txn_id and TxnManager *txn_mgr
Tuple<DBEntry *, Status> DBMeta::CreateNewEntry(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type) {
    std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker_);
    DBEntry *db_entry_ptr{nullptr};
    //    rw_locker_.lock();
    if (this->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        this->entry_list_.emplace_back(std::move(dummy_entry));

        // Insert the new db entry
        // physical wal log
        SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(this->data_dir_, this->db_name_, txn_id, begin_ts);

        // Physical log
        {
            if (txn_mgr != nullptr) {
                auto operation = MakeUnique<AddDBEntryOp>(db_entry);
                txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
            }
        }

        db_entry_ptr = db_entry.get();
        this->entry_list_.emplace_front(std::move(db_entry));

        LOG_TRACE("New database entry is added.");
        return {db_entry_ptr, Status::OK()};
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry *header_base_entry = this->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            // physical wal log
            SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(this->data_dir_, this->db_name_, txn_id, begin_ts);

            // Physical log
            {
                if (txn_mgr != nullptr) {
                    auto operation = MakeUnique<AddDBEntryOp>(db_entry);
                    txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                }
            }

            db_entry_ptr = db_entry.get();
            this->entry_list_.emplace_front(std::move(db_entry));
            return {db_entry_ptr, Status::OK()};
        }

        DBEntry *header_db_entry = (DBEntry *)header_base_entry;
        if (header_db_entry->Committed()) {
            if (begin_ts > header_db_entry->commit_ts_) {
                if (header_db_entry->deleted_) {
                    // No conflict

                    SharedPtr<DBEntry> db_entry = DBEntry::DBEntry::NewDBEntry(this->data_dir_, this->db_name_, txn_id, begin_ts);
                    {
                        if (txn_mgr != nullptr) {
                            auto operation = MakeUnique<AddDBEntryOp>(db_entry);
                            txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                        }
                    }

                    db_entry_ptr = db_entry.get();
                    this->entry_list_.emplace_front(std::move(db_entry));
                    return {db_entry_ptr, Status::OK()};
                } else {
                    switch (conflict_type) {
                        case ConflictType::kIgnore: {
                            db_entry_ptr = header_db_entry;
                            return {db_entry_ptr, Status::OK()};
                        }
                        default: {
                            // Duplicated database
                            UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Duplicated database name: {}.", *this->db_name_));
                            LOG_ERROR(*err_msg);
                            return {nullptr, Status(ErrorCode::kDuplicateDatabaseName, std::move(err_msg))};
                        }
                    }
                }
            } else {
                // Write-Write conflict
                UniquePtr<String> err_msg =
                    MakeUnique<String>(fmt::format("Write-write conflict: There is a committed database which is later than current transaction."));
                LOG_ERROR(*err_msg);
                return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
            }
        } else {

            TxnState head_db_entry_state = txn_mgr->GetTxnState(header_db_entry->txn_id_);

            switch (head_db_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if (header_db_entry->txn_id_ == txn_id) {
                        // Same txn
                        if (header_db_entry->deleted_) {

                            SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(this->data_dir_, this->db_name_, txn_id, begin_ts);
                            if (txn_mgr != nullptr) {
                                auto operation = MakeUnique<AddDBEntryOp>(db_entry);
                                txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                            }

                            db_entry_ptr = db_entry.get();
                            this->entry_list_.emplace_front(std::move(db_entry));
                            return {db_entry_ptr, Status::OK()};
                        } else {
                            UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Duplicated database name: {}.", *this->db_name_));
                            LOG_ERROR(*err_msg);
                            return {nullptr, Status(ErrorCode::kDuplicateDatabaseName, std::move(err_msg))};
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
                    //
                    this->entry_list_.erase(this->entry_list_.begin());

                    // Rollback Txn will not merge the txn catalog delta operations to global catalog delta operations.
                    // So, rollback catalog operations shouldn't be recorded.
                    // But this new Txn catalog delta operations need to be recorded.
                    SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(this->data_dir_, this->db_name_, txn_id, begin_ts);
                    {
                        if (txn_mgr != nullptr) {
                            auto operation = MakeUnique<AddDBEntryOp>(db_entry);
                            txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                        }
                    }

                    db_entry_ptr = db_entry.get();
                    this->entry_list_.emplace_front(std::move(db_entry));
                    return {db_entry_ptr, Status::OK()};
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

Tuple<DBEntry *, Status> DBMeta::DropNewEntry(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {

    DBEntry *db_entry_ptr{nullptr};
    std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty db entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status::EmptyEntryList()};
    }

    BaseEntry *header_base_entry = this->entry_list_.front().get();

    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        UniquePtr<String> err_msg = MakeUnique<String>("No valid db entry.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status::InvalidEntry()};
    }

    DBEntry *header_db_entry = (DBEntry *)header_base_entry;
    if (header_db_entry->Committed()) {
        if (begin_ts > header_db_entry->commit_ts_) {
            // No conflict
            if (header_db_entry->deleted_) {
                UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped before.");
                LOG_TRACE(*err_msg);
                return {nullptr, Status::DBNotExist(*this->db_name_)};
            }

            SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(this->data_dir_, this->db_name_, txn_id, begin_ts);

            // Physical log
            {
                if (txn_mgr != nullptr) {
                    auto operation = MakeUnique<AddDBEntryOp>(db_entry);
                    txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                }
            }

            db_entry_ptr = db_entry.get();
            db_entry_ptr->deleted_ = true;
            this->entry_list_.emplace_front(std::move(db_entry));

            return {db_entry_ptr, Status::OK()};
        } else {
            // Write-Write conflict
            UniquePtr<String> err_msg =
                MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_db_entry->txn_id_) {
            // Same txn, remove the header db entry
            auto base_entry_ptr = this->entry_list_.front();
            auto db_entry = std::static_pointer_cast<DBEntry>(base_entry_ptr);
            {
                if (txn_mgr != nullptr) {
                    auto operation = MakeUnique<AddDBEntryOp>(db_entry);
                    txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                }
            }

            this->entry_list_.pop_front();
            return {db_entry.get(), Status::OK()};
        } else {
            // Not same txn, issue WW conflict
            UniquePtr<String> err_msg = MakeUnique<String>("Write-write conflict: There is another uncommitted db entry.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
        }
    }
}

void DBMeta::AddEntry(DBMeta *db_meta, UniquePtr<BaseEntry> db_entry) {
    std::unique_lock<std::shared_mutex> rw_locker(db_meta->rw_locker_);
    db_meta->entry_list_.emplace_front(std::move(db_entry));
}

void DBMeta::DeleteNewEntry(TransactionID txn_id, TxnManager *txn_mgr) {
    std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.");
        return;
    }

    auto removed_iter =
        std::remove_if(this->entry_list_.begin(), this->entry_list_.end(), [&](auto &entry) -> bool { return entry->txn_id_ == txn_id; });
    // DeleteNewEntry is only called when txn is rollback, so no need to add physical log
    this->entry_list_.erase(removed_iter, this->entry_list_.end());
}

Tuple<DBEntry *, Status> DBMeta::GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts) {
    std::shared_lock<std::shared_mutex> r_locker(this->rw_locker_);
    if (this->entry_list_.empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty db entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status::EmptyEntryList()};
    }

    for (const auto &db_entry : this->entry_list_) {
        if (db_entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid db entry.");
            LOG_ERROR(*err_msg);
            return {nullptr, Status::InvalidEntry()};
        }

        if (db_entry->Committed()) {
            if (begin_ts > db_entry->commit_ts_) {
                if (db_entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped.");
                    LOG_TRACE(*err_msg);
                    return {nullptr, Status::DBNotExist(*this->db_name_)};
                } else {
                    // check the tables meta
                    auto db_entry_ptr = static_cast<DBEntry *>(db_entry.get());
                    return {db_entry_ptr, Status::OK()};
                }
            }
        } else {
            // Only committed txn is visible. Committing txn isn't visble,
            // except same txn is visible
            if (txn_id == db_entry->txn_id_) {
                if (db_entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("DB is dropped.");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status::DBNotExist(*this->db_name_)};
                } else {
                    auto db_entry_ptr = static_cast<DBEntry *>(db_entry.get());
                    return {db_entry_ptr, Status::OK()};
                }
            }
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("No db entry {} found.", *this->db_name_));
    LOG_ERROR(*err_msg);
    return {nullptr, Status::NotFoundEntry()};
}

Tuple<DBEntry *, Status> DBMeta::GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {
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
                auto db_entry = static_cast<DBEntry *>(entry.get());
                return {db_entry, Status::OK()};
            }
        } else {
            if (txn_id == entry_txn_id) {
                auto db_entry = static_cast<DBEntry *>(entry.get());
                return {db_entry, Status::OK()};
            }
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>("No db entry found.");
    LOG_ERROR(*err_msg);
    return {nullptr, Status::NotFoundEntry()};
}

SharedPtr<String> DBMeta::ToString() {
    std::shared_lock<std::shared_mutex> r_locker(this->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(
        fmt::format("DBMeta, data_dir: {}, db name: {}, entry count: ", *this->data_dir_, *this->db_name_, this->entry_list_.size()));
    return res;
}

nlohmann::json DBMeta::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;
    Vector<DBEntry *> db_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json_res["data_dir"] = *this->data_dir_;
        json_res["db_name"] = *this->db_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        db_candidates.reserve(this->entry_list_.size());
        for (auto &db_entry : this->entry_list_) {
            if (db_entry->entry_type_ == EntryType::kDatabase) {
                // Put it to candidate list
                db_candidates.push_back((DBEntry *)db_entry.get());
            }
        }
    }
    for (DBEntry *db_entry : db_candidates) {
        json_res["db_entries"].emplace_back(db_entry->Serialize(max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

UniquePtr<DBMeta> DBMeta::Deserialize(const nlohmann::json &db_meta_json, BufferManager *buffer_mgr) {
    SharedPtr<String> data_dir = MakeShared<String>(db_meta_json["data_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_meta_json["db_name"]);
    UniquePtr<DBMeta> res = MakeUnique<DBMeta>(data_dir, db_name);

    if (db_meta_json.contains("db_entries")) {
        for (const auto &db_entry_json : db_meta_json["db_entries"]) {
            res->entry_list_.emplace_back(DBEntry::Deserialize(db_entry_json, buffer_mgr));
        }
    }
    res->entry_list_.sort([](const SharedPtr<BaseEntry> &ent1, const SharedPtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });
    auto dummy_entry = MakeShared<BaseEntry>(EntryType::kDummy);
    res->entry_list_.emplace_back(dummy_entry);
    return res;
}

void DBMeta::MergeFrom(DBMeta &other) {
    // No locking here since only the load stage needs MergeFrom.
    if (!IsEqual(*this->db_name_, *other.db_name_)) {
        UnrecoverableError("DBMeta::MergeFrom requires db_name_ match");
    }
    if (!IsEqual(*this->data_dir_, *other.data_dir_)) {
        UnrecoverableError("DBMeta::MergeFrom requires db_dir_ match");
    }
    MergeLists(this->entry_list_, other.entry_list_);
}

} // namespace infinity

//
// Created by jinhai on 23-6-4.
//

#include "db_meta.h"
#include "common/types/internal_types.h"
#include "main/logger.h"
#include "common/utility/defer_op.h"
#include "storage/txn/txn_manager.h"

namespace infinity {

EntryResult
DBMeta::CreateNewEntry(DBMeta* db_meta,
                       u64 txn_id,
                       TxnTimeStamp begin_ts,
                       TxnManager* txn_mgr) {
    DBEntry* res = nullptr;
    std::unique_lock<RWMutex> rw_locker(db_meta->rw_locker_);

    SharedPtr<String> meta_dir = MakeShared<String>(*db_meta->base_dir_ + '/' + *db_meta->db_name_);
//    rw_locker_.lock();
    if(db_meta->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        db_meta->entry_list_.emplace_back(std::move(dummy_entry));

        // Insert the new db entry
        UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(meta_dir,
                                                          db_meta->db_name_,
                                                          txn_id,
                                                          begin_ts);
        res = db_entry.get();
        db_meta->entry_list_.emplace_front(std::move(db_entry));


        LOG_TRACE("New database entry is added.");
        return {res, nullptr};
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry* header_base_entry = db_meta->entry_list_.front().get();
        if(header_base_entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(meta_dir,
                                                              db_meta->db_name_,
                                                              txn_id,
                                                              begin_ts);
            res = db_entry.get();
            db_meta->entry_list_.emplace_front(std::move(db_entry));
            return {res, nullptr};
        }

        DBEntry* header_db_entry = (DBEntry*)header_base_entry;
        if(header_db_entry->Committed()) {
            if(begin_ts > header_db_entry->commit_ts_) {
                if(header_db_entry->deleted_) {
                    // No conflict
                    UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(meta_dir,
                                                                      db_meta->db_name_,
                                                                      txn_id,
                                                                      begin_ts);
                    res = db_entry.get();
                    db_meta->entry_list_.emplace_front(std::move(db_entry));
                    return {res, nullptr};
                } else {
                    // Duplicated database
                    LOG_TRACE("Duplicated database name: {}.", *db_meta->db_name_)
                    return {nullptr, MakeUnique<String>("Duplicated database.")};
                }
            } else {
                // Write-Write conflict
                LOG_TRACE("Write-write conflict: There is a committed database which is later than current transaction.")
                return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
            }
        } else {

            TxnState head_db_entry_state = txn_mgr->GetTxnState(header_db_entry->txn_id_);

            switch(head_db_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if(header_db_entry->txn_id_ == txn_id) {
                        // Same txn
                        if(header_db_entry->deleted_) {
                            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(meta_dir,
                                                                              db_meta->db_name_,
                                                                              txn_id,
                                                                              begin_ts);
                            res = db_entry.get();
                            db_meta->entry_list_.emplace_front(std::move(db_entry));
                            return {res, nullptr};
                        } else {
                            LOG_TRACE("Create a duplicated name database.")
                            return {nullptr, MakeUnique<String>("Create a duplicated name database.")};
                        }
                    } else {
                        LOG_TRACE("Write-write conflict: There is a uncommitted transaction.")
                        return {nullptr, MakeUnique<String>("Write-write conflict: There is a uncommitted transaction.")};
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    LOG_TRACE("Write-write conflict: There is a committing/committed database which is later than current transaction.")
                    return {nullptr, MakeUnique<String>("Write-write conflict: There is a committing/committed database which is later than current transaction.")};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    db_meta->entry_list_.erase(db_meta->entry_list_.begin());

                    // Append new one
                    UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(meta_dir,
                                                                      db_meta->db_name_,
                                                                      txn_id,
                                                                      begin_ts);
                    res = db_entry.get();
                    db_meta->entry_list_.emplace_front(std::move(db_entry));
                    return {res, nullptr};
                }
                default: {
                    LOG_TRACE("Invalid db entry txn state")
                    return {nullptr, MakeUnique<String>("Invalid db entry txn state.")};
                }
            }
        }
    }
}

EntryResult
DBMeta::DropNewEntry(DBMeta* db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager* txn_mgr) {
    DBEntry* res = nullptr;
    std::unique_lock<RWMutex> rw_locker(db_meta->rw_locker_);
    if(db_meta->entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.")
        return {nullptr, MakeUnique<String>("Empty db entry list.")};
    }

    BaseEntry* header_base_entry = db_meta->entry_list_.front().get();
    if(header_base_entry->entry_type_ == EntryType::kDummy) {
//            rw_locker_.unlock();
        LOG_TRACE("No valid db entry.")
        return {nullptr, MakeUnique<String>("No valid db entry.")};
    }

    DBEntry* header_db_entry = (DBEntry*)header_base_entry;
    if(header_db_entry->Committed()) {
        if(begin_ts > header_db_entry->commit_ts_) {
            // No conflict
            if(header_db_entry->deleted_) {
                LOG_TRACE("DB is dropped before.")
                return {nullptr, MakeUnique<String>("DB is dropped before.")};
            }

            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta->base_dir_,
                                                              db_meta->db_name_,
                                                              txn_id,
                                                              begin_ts);
            res = db_entry.get();
            res->deleted_ = true;
            db_meta->entry_list_.emplace_front(std::move(db_entry));

            return {res, nullptr};
        } else {
            // Write-Write conflict
            LOG_TRACE("Write-write conflict: There is a committed database which is later than current transaction.");
            return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
        }
    } else {
        // Uncommitted, check if the same txn
        if(txn_id == header_db_entry->txn_id_) {
            // Same txn, remove the header db entry
            res = header_db_entry;
            db_meta->entry_list_.erase(db_meta->entry_list_.begin());

            return {res, nullptr};
        } else {
            // Not same txn, issue WW conflict
            LOG_TRACE("Write-write conflict: There is another uncommitted db entry.")
            return {nullptr, MakeUnique<String>("Write-write conflict: There is another uncommitted db entry.")};
        }
    }
}

void
DBMeta::DeleteNewEntry(DBMeta* db_meta, u64 txn_id, TxnManager* txn_mgr) {
    std::unique_lock<RWMutex> rw_locker(db_meta->rw_locker_);
    if(db_meta->entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.")
        return ;
    }

    auto removed_iter = std::remove_if(db_meta->entry_list_.begin(),
                                       db_meta->entry_list_.end(),
                                       [&](auto& entry)->bool {
        return entry->txn_id_ == txn_id;
    });

    db_meta->entry_list_.erase(removed_iter, db_meta->entry_list_.end());
}

EntryResult
DBMeta::GetEntry(DBMeta* db_meta, u64 txn_id, TxnTimeStamp begin_ts) {
    std::shared_lock<RWMutex> r_locker(db_meta->rw_locker_);
    if(db_meta->entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.")
        return {nullptr, MakeUnique<String>("Empty db entry list.")};
    }


    for(const auto& db_entry: db_meta->entry_list_) {
        if(db_entry->entry_type_ == EntryType::kDummy) {
            LOG_TRACE("No valid db entry.")
            return {nullptr, MakeUnique<String>("No valid db entry.")};
        }

        if(db_entry->Committed()) {
            if(begin_ts > db_entry->commit_ts_) {
                if(db_entry->deleted_) {
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
            if(txn_id == db_entry->txn_id_ ) {
                if (db_entry->deleted_) {
//                    LOG_TRACE("DB is dropped.")
                    return {nullptr, MakeUnique<String>("DB is dropped.")};
                } else {
                    return {db_entry.get(), nullptr};
                }
            }
        }
    }
    LOG_TRACE("No db entry found.")
    return {nullptr, MakeUnique<String>("No db entry found.")};
}

SharedPtr<String>
DBMeta::ToString(DBMeta* db_meta) {
    std::shared_lock<RWMutex> r_locker(db_meta->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(fmt::format("DBMeta, base dir: {}, db name: {}, entry count: ",
                                                           *db_meta->base_dir_,
                                                           *db_meta->db_name_,
                                                           db_meta->entry_list_.size()));
    return res;
}

nlohmann::json
DBMeta::Serialize(const DBMeta* db_meta) {
    nlohmann::json json_res;

    json_res["base_dir"] = *db_meta->base_dir_;
    json_res["db_name"] = *db_meta->db_name_;
    for(const auto& base_entry: db_meta->entry_list_) {
        if(base_entry->entry_type_ == EntryType::kDatabase) {
            DBEntry* db_entry = (DBEntry*)base_entry.get();
            json_res["entries"].emplace_back(DBEntry::Serialize(db_entry));
        } else if(base_entry->entry_type_ == EntryType::kDummy) {
            ; // LOG_TRACE("Skip dummy type entry during serialize database {} meta", *db_meta->db_name_);
        } else {
            StorageError("Unexpected entry type");
        }
    }
    return json_res;
}

UniquePtr<DBMeta>
DBMeta::Deserialize(const nlohmann::json& db_meta_json,
                    BufferManager* buffer_mgr) {
    SharedPtr<String> base_dir = MakeShared<String>(db_meta_json["base_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_meta_json["db_name"]);
    UniquePtr<DBMeta> res = MakeUnique<DBMeta>(base_dir, db_name);

    if(db_meta_json.contains("entries")) {
        for(const auto& db_entry_json: db_meta_json["entries"]) {
            res->entry_list_.emplace_back(DBEntry::Deserialize(db_entry_json, buffer_mgr));
        }
    }
    return res;
}

}

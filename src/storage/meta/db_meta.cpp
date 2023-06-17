//
// Created by jinhai on 23-6-4.
//

#include "db_meta.h"
#include "main/logger.h"

namespace infinity {

EntryResult
DBMeta::CreateNewEntry(u64 txn_id, TxnTimeStamp begin_ts) {
    DBEntry* res = nullptr;
    std::unique_lock<RWMutex> rw_locker(rw_locker_);
//    rw_locker_.lock();
    if(entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kInvalid);
        dummy_entry->deleted_ = true;
        entry_list_.emplace_back(std::move(dummy_entry));

        // Insert the new db entry
        UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_name_, txn_id, begin_ts);
        res = db_entry.get();
        entry_list_.emplace_front(std::move(db_entry));

//        rw_locker_.unlock();
        LOG_TRACE("New database entry is added.");
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry* header_base_entry = entry_list_.front().get();
        if(header_base_entry->entry_type_ != EntryType::kDatabase) {
//            rw_locker_.unlock();
            LOG_TRACE("Invalid entry type of meta data")
            return {nullptr, MakeUnique<String>("Invalid entry type of meta data")};
        }

        DBEntry* header_db_entry = (DBEntry*)header_base_entry;
        if(header_db_entry->commit_ts_ < UNCOMMIT_TS) {
            // Committed
            if(begin_ts > header_db_entry->commit_ts_) {
                // No conflict
                UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_name_, txn_id, begin_ts);
                res = db_entry.get();
                entry_list_.emplace_front(std::move(db_entry));
            } else {
                // Write-Write conflict
                LOG_TRACE("Write-write conflict: There is a committed database which is later than current transaction.")
                return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
            }
        } else {
            // TODO: To battle which txn can survive.
//            rw_locker_.unlock();
            // Uncommitted, Write-Write conflict
            LOG_TRACE("Write-write conflict: There is a uncommitted database.")
            return {nullptr, MakeUnique<String>("Write-write conflict: There is a uncommitted database.")};
        }
    }

    return {res, nullptr};
}

EntryResult
DBMeta::DeleteNewEntry(u64 txn_id, TxnTimeStamp begin_ts) {
    DBEntry* res = nullptr;
    std::unique_lock<RWMutex> rw_locker(rw_locker_);
    if(entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.")
        return {nullptr, MakeUnique<String>("Empty db entry list.")};
    }

    BaseEntry* header_base_entry = entry_list_.front().get();
    if(header_base_entry->entry_type_ != EntryType::kDatabase) {
//            rw_locker_.unlock();
        LOG_TRACE("No valid db entry.")
        return {nullptr, MakeUnique<String>("No valid db entry.")};
    }

    DBEntry* header_db_entry = (DBEntry*)header_base_entry;
    if(header_db_entry->commit_ts_ < UNCOMMIT_TS) {
        // Committed
        if(begin_ts > header_db_entry->commit_ts_) {
            // No conflict
            UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_name_, txn_id, begin_ts);
            res = db_entry.get();
            res->deleted_ = true;
            entry_list_.emplace_front(std::move(db_entry));

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
            entry_list_.erase(entry_list_.begin());

            return {res, nullptr};
        } else {
            // Not same txn, issue WW conflict
            LOG_TRACE("Write-write conflict: There is another uncommitted db entry.")
            return {nullptr, MakeUnique<String>("Write-write conflict: There is another uncommitted db entry.")};
        }
    }
}

EntryResult
DBMeta::GetEntry(u64 txn_id, TxnTimeStamp begin_ts) {
//    DBEntry* res = nullptr;
    std::shared_lock<RWMutex> r_locker(rw_locker_);
    if(entry_list_.empty()) {
        LOG_TRACE("Empty db entry list.")
        return {nullptr, MakeUnique<String>("Empty db entry list.")};
    }


    for(const auto& db_entry: entry_list_) {
        if(db_entry->entry_type_ != EntryType::kDatabase) {
            LOG_TRACE("No valid db entry.")
            return {nullptr, MakeUnique<String>("No valid db entry.")};
        }

        if(db_entry->commit_ts_ < UNCOMMIT_TS) {
            // committed
            if(begin_ts > db_entry->commit_ts_) {
                return {db_entry.get(), nullptr};
            }
        } else {
            // not committed
            if(txn_id == db_entry->txn_id_) {
                // same txn
                return {db_entry.get(), nullptr};
            }
        }
    }
    LOG_TRACE("No db entry found.")
    return {nullptr, MakeUnique<String>("No db entry found.")};
}

}

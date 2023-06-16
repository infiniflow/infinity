//
// Created by jinhai on 23-6-4.
//

#include "db_meta.h"
#include "main/logger.h"

namespace infinity {

DBEntry*
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
            LOG_ERROR("Invalid entry type of meta data")
            return nullptr;
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
                LOG_ERROR("Write-write conflict: There is a committed database which is later than current transaction.")
                return nullptr;
            }
        } else {
            // TODO: To battle which txn can survive.
//            rw_locker_.unlock();
            // Uncommitted, Write-Write conflict
            LOG_ERROR("Write-write conflict: There is a uncommitted database.")
            return nullptr;
        }
    }

    return res;
}

}

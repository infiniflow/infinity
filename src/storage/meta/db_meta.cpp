//
// Created by jinhai on 23-6-4.
//

#include "db_meta.h"

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
        entry_list_.emplace_back(std::move(db_entry));

        res = db_entry.get();
//        rw_locker_.unlock();
    } else {
        // Already have a db_entry, check if the db_entry is valid here.
        BaseEntry* header_base_entry = entry_list_.front().get();
        if(header_base_entry->entry_type_ != EntryType::kDatabase) {
//            rw_locker_.unlock();
            StorageError("Invalid entry type of meta data")
        }

        DBEntry* header_db_entry = (DBEntry*)header_base_entry;
        if(header_db_entry->commit_ts_ < UNCOMMIT_TS) {
            // Committed
            if(begin_ts > header_db_entry->commit_ts_) {
                // No conflict
                UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_name_, txn_id, begin_ts);
                entry_list_.emplace_back(std::move(db_entry));
                res = db_entry.get();
            } else {
                // Write-Write conflict
                StorageError("Write-write conflict: There is a committed database which is later than current transaction.")
            }
        } else {
            // TODO: To battle which txn can survive.
//            rw_locker_.unlock();
            // Uncommitted, Write-Write conflict
            StorageError("Write-write conflict: There is a uncommitted database.")
        }
    }

    return res;
}

}

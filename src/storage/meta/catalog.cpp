//
// Created by jinhai on 23-6-4.
//

#include "catalog.h"
#include "main/logger.h"

namespace infinity {

NewCatalog::NewCatalog(UniquePtr<String> dir, UniquePtr<AsyncBatchProcessor> scheduler)
    : dir_(std::move(dir)), scheduler_(std::move(scheduler)) {
}

EntryResult
NewCatalog::CreateDatabase(const String& db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context) {
    // Check if there is db_meta with the db_name
    rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) != databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_locker_.unlock_shared();

    // no db_meta
    if(db_meta == nullptr) {
        // Create new db meta
        LOG_TRACE("Create new database {}", db_name);
        UniquePtr<DBMeta> new_db_meta = MakeUnique<DBMeta>(db_name);
        db_meta = new_db_meta.get();

        rw_locker_.lock();
        databases_[db_name] = std::move(new_db_meta);
        rw_locker_.unlock();

    }

    LOG_TRACE("Add new database entry {}", db_name);
    EntryResult res = db_meta->CreateNewEntry(txn_id, begin_ts, txn_context);

    return res;
}

EntryResult
NewCatalog::DropDatabase(const String& db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context) {
    rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) != databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_locker_.unlock_shared();
    if(db_meta == nullptr) {
        LOG_TRACE("Attempt to drop not existed database entry {}", db_name);
        return {nullptr, MakeUnique<String>("Attempt to drop not existed database entry")};
    }

    LOG_TRACE("Drop a database entry {}", db_name);
    EntryResult res = db_meta->DropNewEntry(txn_id, begin_ts, txn_context);

    return res;
}

EntryResult
NewCatalog::GetDatabase(const String& db_name, u64 txn_id, TxnTimeStamp begin_ts) {
    rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) != databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_locker_.unlock_shared();

    LOG_TRACE("Get a database entry {}", db_name);
    return db_meta->GetEntry(txn_id, begin_ts);
}

void
NewCatalog::RemoveDBEntry(const String& db_name, u64 txn_id, TxnContext* txn_context) {
    rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) != databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_locker_.unlock_shared();

    LOG_TRACE("Remove a database entry {}", db_name);
    db_meta->DeleteNewEntry(txn_id, txn_context);
}

}

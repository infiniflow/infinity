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
    rw_lock_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) != databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_lock_.unlock_shared();

    // no db_meta
    if(db_meta == nullptr) {
        // Create new db meta
        LOG_TRACE("Create new database");
        UniquePtr<DBMeta> new_db_meta = MakeUnique<DBMeta>(db_name);
        db_meta = new_db_meta.get();

        rw_lock_.lock();
        databases_[db_name] = std::move(new_db_meta);
        rw_lock_.unlock();

    }

    LOG_TRACE("Add new database entry");
    EntryResult res = db_meta->CreateNewEntry(txn_id, begin_ts, txn_context);

    return res;
}

EntryResult
NewCatalog::DropDatabase(const String& db_name, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context) {
    rw_lock_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) != databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_lock_.unlock_shared();
    if(db_meta == nullptr) {
        LOG_TRACE("Attempt to drop not existed database entry");
        return {nullptr, MakeUnique<String>("Attempt to drop not existed database entry")};
    }

    LOG_TRACE("Drop a database entry");
    EntryResult res = db_meta->DropNewEntry(txn_id, begin_ts, txn_context);

    return res;
}

EntryResult
NewCatalog::GetDatabase(const String& db_name, u64 txn_id, TxnTimeStamp begin_ts) {
    rw_lock_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) != databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_lock_.unlock_shared();

    return db_meta->GetEntry(txn_id, begin_ts);
}

void
NewCatalog::RemoveDBEntry(const String& db_name, u64 txn_id, TxnContext* txn_context) {
    rw_lock_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) != databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_lock_.unlock_shared();

    db_meta->DeleteNewEntry(txn_id, txn_context);
}

}

//
// Created by jinhai on 23-6-4.
//

#include "catalog.h"
#include "main/logger.h"

namespace infinity {

NewCatalog::NewCatalog(UniquePtr<String> dir, UniquePtr<AsyncBatchProcessor> scheduler)
    : dir_(std::move(dir)), scheduler_(std::move(scheduler)) {
}

DBEntry*
NewCatalog::CreateDatabase(const String& db_name, u64 txn_id, TxnTimeStamp begin_ts) {
    // Check if there is db_meta with the db_name
    rw_lock_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(databases_.find(db_name) == databases_.end()) {
        db_meta = databases_[db_name].get();
    }
    rw_lock_.unlock_shared();

    // no db_meta
    if(db_meta == nullptr) {
        // Create new db meta
        rw_lock_.lock();
        LOG_TRACE("Create new database");
        UniquePtr<DBMeta> new_db_meta = MakeUnique<DBMeta>(db_name);
        db_meta = new_db_meta.get();
        databases_[db_name] = std::move(new_db_meta);
        rw_lock_.unlock();

    }

    LOG_TRACE("Add new database entry");
    db_meta->CreateNewEntry(txn_id, begin_ts);

    return nullptr;
}

DBEntry*
NewCatalog::DropDatabase(const String& name, u64 txn_id, TxnTimeStamp begin_ts) {
    return nullptr;
}


}



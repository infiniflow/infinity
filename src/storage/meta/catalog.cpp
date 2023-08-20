//
// Created by jinhai on 23-6-4.
//

#include "catalog.h"
#include "main/logger.h"

namespace infinity {

NewCatalog::NewCatalog(SharedPtr<String> dir, UniquePtr<AsyncBatchProcessor> scheduler)
    : current_dir_(std::move(dir)), scheduler_(std::move(scheduler)) {
}

EntryResult
NewCatalog::CreateDatabase(NewCatalog* catalog,
                           const String& db_name,
                           u64 txn_id,
                           TxnTimeStamp begin_ts,
                           TxnContext* txn_context) {

    // Check if there is db_meta with the db_name
    catalog->rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

    // no db_meta
    if(db_meta == nullptr) {
        // Create new db meta
        LOG_TRACE("Create new database: {}", db_name);
        UniquePtr<DBMeta> new_db_meta = MakeUnique<DBMeta>(catalog->current_dir_, db_name);
        db_meta = new_db_meta.get();

        catalog->rw_locker_.lock();
        catalog->databases_[db_name] = std::move(new_db_meta);
        catalog->rw_locker_.unlock();

    }

    LOG_TRACE("Add new database entry: {}", db_name);
    EntryResult res = DBMeta::CreateNewEntry(db_meta, txn_id, begin_ts, txn_context);

    return res;
}

EntryResult
NewCatalog::DropDatabase(NewCatalog* catalog,
                         const String& db_name,
                         u64 txn_id,
                         TxnTimeStamp begin_ts,
                         TxnContext* txn_context) {

    catalog->rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();
    if(db_meta == nullptr) {
        LOG_TRACE("Attempt to drop not existed database entry {}", db_name);
        return {nullptr, MakeUnique<String>("Attempt to drop not existed database entry")};
    }

    LOG_TRACE("Drop a database entry {}", db_name);
    EntryResult res = DBMeta::DropNewEntry(db_meta, txn_id, begin_ts, txn_context);

    return res;

}

EntryResult
NewCatalog::GetDatabase(NewCatalog* catalog,
                        const String& db_name,
                        u64 txn_id,
                        TxnTimeStamp begin_ts) {
    catalog->rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

    LOG_TRACE("Get a database entry {}", db_name);
    return DBMeta::GetEntry(db_meta, txn_id, begin_ts);
}

void
NewCatalog::RemoveDBEntry(NewCatalog* catalog,
                          const String& db_name,
                          u64 txn_id,
                          TxnContext* txn_context) {
    catalog->rw_locker_.lock_shared();

    DBMeta* db_meta{nullptr};
    if(catalog->databases_.find(db_name) != catalog->databases_.end()) {
        db_meta = catalog->databases_[db_name].get();
    }
    catalog->rw_locker_.unlock_shared();

    LOG_TRACE("Remove a database entry {}", db_name);
    DBMeta::DeleteNewEntry(db_meta, txn_id, txn_context);
}

Vector<DBEntry*>
NewCatalog::Databases(NewCatalog* catalog, Txn* txn) {
    NotImplementError("NewCatalog::Databases isn't implemented.")
}

}

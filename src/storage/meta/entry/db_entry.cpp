//
// Created by jinhai on 23-6-23.
//

#include "db_entry.h"

namespace infinity {

EntryResult
DBEntry::CreateTable(DBEntry* db_entry,
                     const SharedPtr<TableDef>& table_def,
                     u64 txn_id,
                     TxnTimeStamp begin_ts,
                     TxnContext* txn_context) {
    const String& table_name = table_def->table_name();

    // Check if there is table_meta with the table_name
    db_entry->rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(db_entry->tables_.find(table_name) != db_entry->tables_.end()) {
        table_meta = db_entry->tables_[table_name].get();
    }
    db_entry->rw_locker_.unlock_shared();

    // no table_meta
    if(table_meta == nullptr) {
        // Create new db meta
        LOG_TRACE("Create new table: {}", table_name);
        SharedPtr<String> db_entry_dir = MakeShared<String>((*db_entry->base_dir_) + "/txn_" + std::to_string(txn_id));
        UniquePtr<TableMeta> new_table_meta = MakeUnique<TableMeta>(db_entry_dir, table_name, db_entry);
        table_meta = new_table_meta.get();

        db_entry->rw_locker_.lock();
        db_entry->tables_[table_name] = std::move(new_table_meta);
        db_entry->rw_locker_.unlock();

        LOG_TRACE("Add new database entry for: {} in new table meta: {} ", table_name, *db_entry->base_dir_);
        EntryResult res = TableMeta::CreateNewEntry(table_meta,
                                                    txn_id,
                                                    begin_ts,
                                                    txn_context,
                                                    table_def);
        return res;
    } else {
        LOG_TRACE("Add new database entry for: {} in existed table meta: {}", table_name, *db_entry->base_dir_);
        EntryResult res = TableMeta::CreateNewEntry(table_meta,
                                                    txn_id,
                                                    begin_ts,
                                                    txn_context,
                                                    table_def);
        return res;
    }
}

EntryResult
DBEntry::DropTable(DBEntry* db_entry,
                   const String& table_name,
                   u64 txn_id,
                   TxnTimeStamp begin_ts,
                   TxnContext* txn_context) {
    db_entry->rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(db_entry->tables_.find(table_name) != db_entry->tables_.end()) {
        table_meta = db_entry->tables_[table_name].get();
    }
    db_entry->rw_locker_.unlock_shared();
    if(table_meta == nullptr) {
        LOG_TRACE("Attempt to drop not existed table entry {}", table_name);
        return {nullptr, MakeUnique<String>("Attempt to drop not existed table entry")};
    }

    LOG_TRACE("Drop a table entry {}", table_name);
    EntryResult res = TableMeta::DropNewEntry(table_meta,
                                              txn_id,
                                              begin_ts,
                                              txn_context,
                                              table_name);

    return res;
}


EntryResult
DBEntry::GetTable(DBEntry* db_entry,
                  const String& table_name,
                  u64 txn_id,
                  TxnTimeStamp begin_ts) {
    db_entry->rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(db_entry->tables_.find(table_name) != db_entry->tables_.end()) {
        table_meta = db_entry->tables_[table_name].get();
    }
    db_entry->rw_locker_.unlock_shared();

    LOG_TRACE("Get a table entry {}", table_name);
    return TableMeta::GetEntry(table_meta, txn_id, begin_ts);
}


void
DBEntry::RemoveTableEntry(DBEntry* db_entry,
                          const String& table_name,
                          u64 txn_id,
                          TxnContext* txn_context) {
    db_entry->rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(db_entry->tables_.find(table_name) != db_entry->tables_.end()) {
        table_meta = db_entry->tables_[table_name].get();
    }
    db_entry->rw_locker_.unlock_shared();

    LOG_TRACE("Remove a table entry: {}", table_name);
    TableMeta::DeleteNewEntry(table_meta, txn_id, txn_context);
}

SharedPtr<String>
DBEntry::ToString(DBEntry* db_entry) {
    std::shared_lock<RWMutex> r_locker(db_entry->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(fmt::format("DBEntry, base dir: {}, txn id: {}, table count: ",
                                                           *db_entry->base_dir_,
                                                           db_entry->txn_id_,
                                                           db_entry->tables_.size()));
    return res;
}

nlohmann::json
DBEntry::Serialize(const DBEntry* db_entry) {
    nlohmann::json json_res;

    json_res["base_dir"] = *db_entry->base_dir_;
    json_res["db_name"] = db_entry->db_name_;
    for(const auto& table_meta_pair: db_entry->tables_) {
        json_res["tables"].emplace_back(TableMeta::Serialize(table_meta_pair.second.get()));
    }
    return json_res;
}

}

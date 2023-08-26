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
                     TxnManager* txn_mgr) {
    const String& table_name = *table_def->table_name();

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
        UniquePtr<TableMeta> new_table_meta = MakeUnique<TableMeta>(db_entry_dir, table_def->table_name(), db_entry);
        table_meta = new_table_meta.get();

        db_entry->rw_locker_.lock();
        db_entry->tables_[table_name] = std::move(new_table_meta);
        db_entry->rw_locker_.unlock();

        LOG_TRACE("Add new database entry for: {} in new table meta: {} ", table_name, *db_entry->base_dir_);
        EntryResult res = TableMeta::CreateNewEntry(table_meta,
                                                    txn_id,
                                                    begin_ts,
                                                    txn_mgr,
                                                    table_def);
        return res;
    } else {
        LOG_TRACE("Add new database entry for: {} in existed table meta: {}", table_name, *db_entry->base_dir_);
        EntryResult res = TableMeta::CreateNewEntry(table_meta,
                                                    txn_id,
                                                    begin_ts,
                                                    txn_mgr,
                                                    table_def);
        return res;
    }
}

EntryResult
DBEntry::DropTable(DBEntry* db_entry,
                   const String& table_name,
                   u64 txn_id,
                   TxnTimeStamp begin_ts,
                   TxnManager* txn_mgr) {
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
                                              txn_mgr,
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
                          TxnManager* txn_mgr) {
    db_entry->rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(db_entry->tables_.find(table_name) != db_entry->tables_.end()) {
        table_meta = db_entry->tables_[table_name].get();
    }
    db_entry->rw_locker_.unlock_shared();

    LOG_TRACE("Remove a table entry: {}", table_name);
    TableMeta::DeleteNewEntry(table_meta, txn_id, txn_mgr);
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
    json_res["db_name"] = *db_entry->db_name_;
    json_res["txn_id"] = db_entry->txn_id_.load();
    json_res["begin_ts"] = db_entry->begin_ts_;
    json_res["commit_ts"] = db_entry->commit_ts_.load();
    json_res["deleted"] = db_entry->deleted_;
    json_res["entry_type"] = db_entry->entry_type_;
    for(const auto& table_meta_pair: db_entry->tables_) {
        json_res["tables"].emplace_back(TableMeta::Serialize(table_meta_pair.second.get()));
    }
    return json_res;
}

UniquePtr<DBEntry>
DBEntry::Deserialize(const nlohmann::json& db_entry_json,
                     BufferManager* buffer_mgr) {
    nlohmann::json json_res;

    SharedPtr<String> base_dir = MakeShared<String>(db_entry_json["base_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_entry_json["db_name"]);
    u64 txn_id = db_entry_json["txn_id"];
    u64 begin_ts = db_entry_json["begin_ts"];
    UniquePtr<DBEntry> res = MakeUnique<DBEntry>(base_dir, db_name, txn_id, begin_ts);

    u64 commit_ts = db_entry_json["commit_ts"];
    bool deleted = db_entry_json["deleted"];
    EntryType entry_type = db_entry_json["entry_type"];
    res->commit_ts_ = commit_ts;
    res->deleted_ = deleted;
    res->entry_type_ = entry_type;

    for(const auto& table_meta_json: db_entry_json["tables"]) {
        UniquePtr<TableMeta> table_meta = TableMeta::Deserialize(table_meta_json, res.get(), buffer_mgr);
        res->tables_.emplace(*table_meta->table_name_, std::move(table_meta));
    }

    return res;
}

}

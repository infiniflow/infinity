//
// Created by jinhai on 23-6-23.
//

#include "db_entry.h"

namespace infinity {

EntryResult
DBEntry::CreateTable(UniquePtr<TableDesc> table_desc,
                     u64 txn_id,
                     TxnTimeStamp begin_ts,
                     TxnContext *txn_context) {
    const String& table_name = table_desc->table_name_;

    // Check if there is table_meta with the table_name
    rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(tables_.find(table_name) != tables_.end()) {
        table_meta = tables_[table_name].get();
    }
    rw_locker_.unlock_shared();

    // no table_meta
    if(table_meta == nullptr) {
        // Create new db meta
        LOG_TRACE("Create new table: {}", table_name);
        UniquePtr<TableMeta> new_table_meta = MakeUnique<TableMeta>(table_name);
        table_meta = new_table_meta.get();

        rw_locker_.lock();
        tables_[table_name] = std::move(new_table_meta);
        rw_locker_.unlock();

    }

    LOG_TRACE("Add new database entry for: {}", table_name);
    EntryResult res = table_meta->CreateNewEntry(txn_id, begin_ts, txn_context, std::move(table_desc), this);

    return res;
}

EntryResult
DBEntry::DropTable(const String& table_name, u64 txn_id, TxnTimeStamp begin_ts, TxnContext* txn_context) {
    rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(tables_.find(table_name) != tables_.end()) {
        table_meta = tables_[table_name].get();
    }
    rw_locker_.unlock_shared();
    if(table_meta == nullptr) {
        LOG_TRACE("Attempt to drop not existed table entry {}", table_name);
        return {nullptr, MakeUnique<String>("Attempt to drop not existed table entry")};
    }

    LOG_TRACE("Drop a table entry {}", table_name);
    EntryResult res = table_meta->DropNewEntry(txn_id, begin_ts, txn_context, table_name, this);

    return res;
}

EntryResult
DBEntry::GetTable(const String& table_name, u64 txn_id, TxnTimeStamp begin_ts) {
    rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(tables_.find(table_name) != tables_.end()) {
        table_meta = tables_[table_name].get();
    }
    rw_locker_.unlock_shared();

    LOG_TRACE("Get a table entry {}", table_name);
    return table_meta->GetEntry(txn_id, begin_ts);
}

void
DBEntry::RemoveTableEntry(const String& table_name, u64 txn_id, TxnContext* txn_context) {
    rw_locker_.lock_shared();

    TableMeta* table_meta{nullptr};
    if(tables_.find(table_name) != tables_.end()) {
        table_meta = tables_[table_name].get();
    }
    rw_locker_.unlock_shared();

    LOG_TRACE("Remove a table entry {}", table_name);
    table_meta->DeleteNewEntry(txn_id, txn_context);
}

}
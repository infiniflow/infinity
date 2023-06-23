//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/meta/entry/db_entry.h"
#include "storage/meta/catalog.h"
#include "txn_manager.h"
#include "txn_context.h"
#include "storage/meta/table_desc.h"
#include "storage/meta/entry/table_entry.h"
#include "txn_store.h"

namespace infinity {

class TxnManager;
class Txn {
public:
    explicit
    Txn(TxnManager* txn_mgr, NewCatalog* catalog, u32 txn_id) : txn_mgr_(txn_mgr), catalog_(catalog), txn_id_(txn_id) {}

    void
    BeginTxn(TxnTimeStamp begin_ts);

    void
    CommitTxn(TxnTimeStamp commit_ts);

    void
    RollbackTxn(TxnTimeStamp abort_ts);

    EntryResult
    CreateDatabase(const String& db_name);

    EntryResult
    DropDatabase(const String& db_name);

    EntryResult
    GetDatabase(const String& db_name);

    EntryResult
    CreateTable(const String& db_name, UniquePtr<TableDesc> table_desc);

    EntryResult
    DropTableByName(const String& db_name, const String& table_name);

    EntryResult
    GetTableByName(const String& db_name, const String& table_name);

    void
    Append(const String& db_name, const String& table_name, SharedPtr<DataBlock> input_block);

    inline u64
    TxnID() const {
        return txn_id_;
    }

private:
    NewCatalog* catalog_{};
    u64 txn_id_{};
    TxnContext txn_context_;

    // Related database
    Set<String> db_names_{};
    Set<String> table_names_{};

    // Txn store
    Set<DBEntry*> txn_dbs_{};
    Set<TableEntry*> txn_tables_{};

    HashMap<String, UniquePtr<TxnTableStore>> txn_tables_store_{};


    TxnManager* txn_mgr_{};
};

}

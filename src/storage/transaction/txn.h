//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/meta/entry/db_entry.h"
#include "storage/meta/catalog.h"
#include "txn_manager.h"

namespace infinity {

class TxnManager;
class Txn {
public:
    explicit
    Txn(TxnManager* txn_mgr, NewCatalog* catalog, u32 txn_id) : txn_mgr_(txn_mgr), catalog_(catalog), txn_id_(txn_id) {}

    inline void
    BeginTxn(TxnTimeStamp begin_ts) {
        begin_ts_ = begin_ts;
    }

    void
    CommitTxn(TxnTimeStamp commit_ts);

    inline void
    AbortTxn() {
        // Abort a transaction.
        aborted = true;

        // For loop to remove the all prepared db

    }

    EntryResult
    CreateDatabase(const String& db_name);

    EntryResult
    DropDatabase(const String& db_name);

    EntryResult
    GetDatabase(const String& db_name);

    inline u64
    TxnID() const {
        return txn_id_;
    }

private:
    NewCatalog* catalog_{};
    TxnTimeStamp begin_ts_{};
    TxnTimeStamp commit_ts_{};

    u64 txn_id_{};
    std::atomic_bool aborted{false};

    // Txn store
    Set<DBEntry*> txn_dbs_{};

    TxnManager* txn_mgr_{};
};

}

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

    inline void
    CommitTxn() {
        // Generate a checkpoint.
    }

    inline void
    AbortTxn() {
        // Abort a transaction.
    }

    DBEntry*
    CreateDatabase(const String& db_name) {
        DBEntry* new_db_entry = catalog_->CreateDatabase(db_name, this->txn_id_, this->begin_ts_);
        if(new_db_entry == nullptr) {
            aborted = true;
            return nullptr;
        }
        create_dbs_.emplace_back(new_db_entry);
        return new_db_entry;
    }

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
    Vector<DBEntry*> create_dbs_{};

    TxnManager* txn_mgr_{};
};

}

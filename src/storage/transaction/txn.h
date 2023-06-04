//
// Created by jinhai on 23-6-4.
//

#pragma once

#include "common/types/internal_types.h"
#include "storage/meta/entry/db_entry.h"
#include "storage/meta/catalog.h"
#include "txn_manager.h"

namespace infinity {

class Txn {
public:
    explicit
    Txn(NewCatalog* catalog, u32 txn_id) : catalog_(catalog), txn_id_(txn_id) {}

    inline void
    BeginTxn(TxnTimeStamp begin_ts) {
        begin_ts_ = begin_ts;
    }

    inline void
    CommitTxn() {
        // Generate a checkpoint
    }

    DBEntry*
    CreateDatabase(const String& db_name) {
        DBEntry* new_db_entry = catalog_->CreateDatabase(db_name, this->txn_id_, this->begin_ts_);
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

    // Txn store
    Vector<DBEntry*> create_dbs_{};
};

}

//
// Created by jinhai on 23-6-4.
//

#include "txn.h"
#include "main/logger.h"

namespace infinity {

DBEntry*
Txn::CreateDatabase(const String& db_name) {
    if(begin_ts_ == 0) {
        LOG_TRACE("Transaction isn't started.")
        return nullptr;
    }
    DBEntry* new_db_entry = catalog_->CreateDatabase(db_name, this->txn_id_, this->begin_ts_);
    if(new_db_entry == nullptr) {
        AbortTxn();
        return nullptr;
    }
    txn_dbs_.insert(new_db_entry);
    return new_db_entry;
}

DBEntry*
Txn::DropDatabase(const String& db_name) {
    if(begin_ts_ == 0) {
        LOG_TRACE("Transaction isn't started.")
        return nullptr;
    }
    DBEntry* drop_db_entry = catalog_->DropDatabase(db_name, this->txn_id_, this->begin_ts_);
    if(drop_db_entry == nullptr) {
        AbortTxn();
        return nullptr;
    }

    return nullptr;
}

void
Txn::CommitTxn(TxnTimeStamp commit_ts) {
    // Generate a checkpoint.
    commit_ts_ = commit_ts;

    for(auto* db_entry: txn_dbs_) {
        db_entry->Commit(commit_ts);
    }
}

}

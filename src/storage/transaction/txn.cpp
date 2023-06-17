//
// Created by jinhai on 23-6-4.
//

#include "txn.h"
#include "main/logger.h"

namespace infinity {

EntryResult
Txn::CreateDatabase(const String& db_name) {
    if(begin_ts_ == 0) {
        LOG_TRACE("Transaction isn't started.")
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }
    EntryResult res = catalog_->CreateDatabase(db_name, this->txn_id_, this->begin_ts_);
    if(res.entry_ == nullptr) {
        return res;
    }

    if(res.entry_->entry_type_ != EntryType::kDatabase) {
        return {nullptr, MakeUnique<String>("Invalid database type")};
    }

    auto* db_entry = static_cast<DBEntry*>(res.entry_);
    txn_dbs_.insert(db_entry);
    return res;
}

EntryResult
Txn::DropDatabase(const String& db_name) {
    if(begin_ts_ == 0) {
        LOG_TRACE("Transaction isn't started.")
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }

    EntryResult res = catalog_->DropDatabase(db_name, this->txn_id_, this->begin_ts_);

    if(res.entry_ == nullptr) {
        return res;
    }

    DBEntry* dropped_db_entry = static_cast<DBEntry*>(res.entry_);

    if(txn_dbs_.contains(dropped_db_entry)) {
        txn_dbs_.emplace(dropped_db_entry);
    } else {
        txn_dbs_.insert(dropped_db_entry);
    }

    return res;
}

EntryResult
Txn::GetDatabase(const String& db_name) {
    if(begin_ts_ == 0) {
        LOG_TRACE("Transaction isn't started.")
        return {nullptr, MakeUnique<String>("Transaction isn't started.")};
    }
    return catalog_->GetDatabase(db_name, this->txn_id_, this->begin_ts_);
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
